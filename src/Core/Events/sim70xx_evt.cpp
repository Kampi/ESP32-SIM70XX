 /*
 * sim70xx_evt.cpp
 *
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX driver for ESP32.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Errors and commissions should be reported to DanielKampert@kampis-elektroecke.de.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <list>

#include "sim70xx_evt.h"
#include "../Queue/sim70xx_queue.h"
#include "../Commands/sim7020_commands.h"

#include "../Arch/ESP32/UART/sim70xx_uart.h"
#include "../Arch/ESP32/Timer/sim70xx_timer.h"
#include "../Arch/ESP32/Logging/sim70xx_logging.h"
#include "../Arch/ESP32/Watchdog/sim70xx_watchdog.h"

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_TASK_CORE_AFFINITY
    #ifndef CONFIG_SIM70XX_TASK_COM_CORE
        #define CONFIG_SIM70XX_TASK_COM_CORE    1
    #endif
#endif

#ifndef CONFIG_SIM70XX_TASK_COM_PRIO
    #define CONFIG_SIM70XX_TASK_COM_PRIO        12
#endif

#ifndef CONFIG_SIM70XX_TASK_COM_STACK
    #define CONFIG_SIM70XX_TASK_COM_STACK       4096
#endif

static const char* TAG = "SIM70XX_Evt";

/** @brief          This task handels the communication with the SIM70XX module.
 *                  The task will receive a reference to a SIM70XX_TxCmd_t object and start with the transmission of the data.
 *                  The reference to the object will be destroyed after the transmission.
 *  @param p_Arg    Pointer to task parameter
 */
static void SIM70XX_Evt_Task(void* p_Arg)
{
    #if(CONFIG_SIMXX_DEV == 7020)
        SIM7020_t* Device = (SIM7020_t*)p_Arg;
    #elif(CONFIG_SIMXX_DEV == 7080)
        SIM7080_t* Device = (SIM7080_t*)p_Arg;
    #endif

    // NOTE: We want to use a list here, because we don??t need random access. The list should be cleared in the same was as it was filled.
    //       Also the list supports iterator invalidation which helps to remove processed commands from the list.
    std::list<SIM70XX_CmdResp_t*> ActiveCommands;

    while(true)
    {
        uint32_t Messages;

        SIM70XX_WDT_Reset();

        // Get all pending messages from the queue.
        Messages = uxQueueMessagesWaiting(Device->Internal.TxQueue);

        if(Device->UART.isInitialized == false)
        {
            continue;
        }

        // Get the commands from the queue and send them.
        for(uint32_t i = 0; i < Messages; i++)
        {
            SIM70XX_WDT_Reset();

            // NOTE: We need pointers, because the object contains a String objects and these objects may be 
            // problematic in FreeRTOS:
            //      https://stackoverflow.com/questions/67346516/using-queue-of-string-in-freertos
            // The application task will create the pointer with the commands and send them to the message
            // queue. The command task will receive the pointer and process them, before releasing the memory.
            SIM70XX_TxCmd_t* CmdObj;

            if(xQueueReceive(Device->Internal.TxQueue, &CmdObj, 0) == pdPASS)
            {
                SIM70XX_CmdResp_t* Command = new SIM70XX_CmdResp_t();

                Command->isError = false;
                Command->isTimeout = false;
                Command->recData = CmdObj->recData;
                Command->Timeout = CmdObj->Timeout * 1000;
                Command->Lines = CmdObj->Lines;
                Command->Length = CmdObj->Command.size();
                ActiveCommands.push_back(Command);

                SIM70XX_LOGI(TAG, "Transmit command: %s", CmdObj->Command.c_str());

                // Transmit the command. The command has the layout
                //  Command<CR><LF>
                SIM70XX_UART_SendLine(Device->UART, CmdObj->Command);

                // Flush the string, because the data aren??t needed anymore.
                CmdObj->Command.clear();

                // Destroy the command.
                delete CmdObj;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        // Asynchronous responses from the module.
        // No messages pending. Get the asynchronous messages.
        if(Messages == 0)
        {
            // New data available?
            if(SIM70XX_UART_Available(Device->UART) > std::string("\r\n").size())
            {
                // NOTE: We have to use dynamic memory here, because the string object can not be used inside queues without issues.
                std::string* Message = new std::string();

                // Get the event message.
                do
                {
                    Message->append(SIM70XX_UART_ReadString(Device->UART));
                } while(SIM70XX_UART_Available(Device->UART));

                SIM70XX_LOGI(TAG, "Event: %s", Message->c_str());

                SIM70XX_Evt_MessageFilter(Device, Message);
            }
        }

        // Synchronous responses from the module.
        // All messages have been send. Now we need to handle the responses. We expect a response for each command.
        std::list<SIM70XX_CmdResp_t*>::iterator it = ActiveCommands.begin();
        while(it != ActiveCommands.end())
        {
            uint32_t Now;

            SIM70XX_WDT_Reset();

            SIM70XX_LOGD(TAG, "Active commands: %u", ActiveCommands.size());

            SIM70XX_UART_ReadStringUntil(Device->UART);

            // Get the data from the command. The transmission has the following layout
            //  <CR><LF>+<Command>: <Data><CR><LF>
            if((*it)->recData)
            {
                // Wait for the device response and process the response.
                Now = SIM70XX_Timer_GetMilliseconds();
                do
                {
                    std::string Line;

                    // Read a new line from the serial interface.
                    // NOTE: The trailing \n is removed from the response!
                    Line = std::string(SIM70XX_UART_ReadStringUntil(Device->UART));

                    // Remove the line ending.
                    SIMXX_TOOLS_REMOVE_LINEEND(Line);

                    if(Line.size() > 0)
                    {
                        SIM70XX_LOGI(TAG, "     Device Response: %s", Line.c_str());
                    }

                    // The device has reported an error.
                    if(Line.find("ERROR") != std::string::npos)
                    {
                        SIM70XX_LOGE(TAG, "     Device Response error!");

                        (*it)->isError = true;

                        break;
                    }
                    // Filter out empty lines.
                    else if(Line.size() > 0)
                    {
                        size_t Index;

                        // Remove the command from the response.
                        Index = Line.find("+");
                        if(Index != std::string::npos)
                        {
                            std::string Dummy;

                            Dummy = Line.substr(Index, Line.find(":") + 1) + " ";
                            Line = Line.replace(Line.find(Dummy), Dummy.size(), "");
                        }

                        // Add the received line to the response when the number of lines is greater than 1.
                        if((*it)->Lines > 1)
                        {
                            (*it)->Response += Line + "\n";
                            (*it)->Lines--;
                        }
                        // Abort when all lines are received.
                        else
                        {
                            (*it)->Response += Line;

                            break;
                        }
                    }
                    // Abort when an empty line was received.
                    else if((Line.size() == 0) && ((*it)->Lines == 0))
                    {
                        break;
                    }
                    // Abort when a timeout occurs.
                    // NOTE: Value 0 will disable the timeout function. This can be used by commands which will report a result anyway.
                    else if(((*it)->Timeout != 0) && (SIM70XX_Timer_GetMilliseconds() - Now) > (*it)->Timeout)
                    {
                        SIM70XX_LOGE(TAG, "     Device Response timout!");

                        (*it)->isTimeout = true;

                        break;
                    }

                    vTaskDelay(10 / portTICK_PERIOD_MS);
                } while(true);
            }

            // The device has not reported an error and no receive timeout was detected. We have to receive the status message now.
            if(((*it)->isError == false) && ((*it)->isTimeout == false))
            {
                // Wait for the status code with the layout
                //  <CR><LF>Status<CR><LF>
                Now = SIM70XX_Timer_GetMilliseconds();
                do
                {
                    std::string Line;

                    SIM70XX_WDT_Reset();

                    Line = SIM70XX_UART_ReadStringUntil(Device->UART);

                    // Read a new line from the serial interface.
                    // NOTE: The trailing \n is removed from the response!
                    (*it)->Status.append(Line);

                    // Abort when a timeout occurs.
                    if(((*it)->Timeout != 0) && (SIM70XX_Timer_GetMilliseconds() - Now) > (*it)->Timeout)
                    {
                        SIM70XX_LOGE(TAG, "     Device status timout!");

                        (*it)->isTimeout = true;

                        break;
                    }

                    vTaskDelay(10 / portTICK_PERIOD_MS);
                } while((*it)->Status.size() < 2);

                // Remove the line endings.
                SIMXX_TOOLS_REMOVE_LINEEND((*it)->Status);

                // SIM7020 outputs "oK" sometimes. Catch this case by converting the status to upper case.
                transform((*it)->Status.begin(), (*it)->Status.end(), (*it)->Status.begin(), ::toupper);

                // Transmission is without error when 'OK' as status code is transmitted and when no event data are received.
                if((*it)->Status.find("OK") == std::string::npos)
                {
                    SIM70XX_LOGE(TAG, "     Device status error!");

                    (*it)->isError = true;
                }

                SIM70XX_LOGI(TAG, "     Device Status: %s", (*it)->Status.c_str());
            }

            // The command was processed completly. Push it to the response queue and remove it from the command list.
            xQueueSend(Device->Internal.RxQueue, &(*it), 0);
            ActiveCommands.erase(it++);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

SIM70XX_Error_t SIM70XX_Evt_StartTask(SIM70XX_UART_Conf_t& p_Config, void* const p_Arg)
{
    if(p_Arg == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

	#ifdef CONFIG_SIM70XX_TASK_CORE_AFFINITY
        xTaskCreatePinnedToCore(SIM70XX_Evt_Task, "SIM70XX_Evt", CONFIG_SIM70XX_TASK_COM_STACK, p_Arg, CONFIG_SIM70XX_TASK_COM_PRIO, &p_Config.TaskHandle, CONFIG_SIM70XX_TASK_COM_CORE);
	#else
        xTaskCreate(SIM70XX_Evt_Task, "SIM70XX_Evt", CONFIG_SIM70XX_TASK_COM_STACK, p_Arg, CONFIG_SIM70XX_TASK_COM_PRIO, &p_Config.TaskHandle);
	#endif
    if(p_Config.TaskHandle == NULL)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(p_Config));

    SIM70XX_WDT_AddHandle(p_Config.TaskHandle);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM70XX_Evt_StopTask(SIM70XX_UART_Conf_t& p_Config)
{
    SIM70XX_ERROR_CHECK(SIM70XX_WDT_RemoveHandle(p_Config.TaskHandle));

    vTaskSuspend(p_Config.TaskHandle);
    vTaskDelete(p_Config.TaskHandle);

    p_Config.TaskHandle = NULL;

    return SIM70XX_UART_Deinit(p_Config);
}