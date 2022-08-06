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

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <list>

#include "sim70xx_evt.h"
#include "../UART/sim70xx_uart.h"
#include "../Queue/sim70xx_queue.h"
#include "../Commands/sim7020_commands.h"

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
    uint32_t StatusUpdate = 0;

    #if(CONFIG_SIMXX_DEV == 7020)
        SIM7020_t* Device = (SIM7020_t*)p_Arg;
    #elif(CONFIG_SIMXX_DEV == 7080)
        SIM7080_t* Device = (SIM7080_t*)p_Arg;
    #endif

    // NOTE: We want to use a list here, because we don´t need random access. The list should be cleared in the same was as it was filled.
    //       Also the list supports iterator invalidation which helps to remove processed commands from the list.
    std::list<SIM70XX_CmdResp_t*> ActiveCommands;

    while(true)
    {
        uint32_t Messages;

        // Get all pending messages from the queue.
        Messages = uxQueueMessagesWaiting(Device->Internal.TxQueue);

        if((SIM70XX_Tools_GetmsTimer() - StatusUpdate) > 1000)
        {
            ESP_LOGD(TAG, "%u messages queued for transmission...", Messages);

            StatusUpdate = SIM70XX_Tools_GetmsTimer();
        }

        if(Device->UART.isInitialized == false)
        {
            continue;
        }

        // Get the commands from the queue and send them.
        for(uint32_t i = 0; i < Messages; i++)
        {
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
                Command->Length = CmdObj->Command.length();
                ActiveCommands.push_back(Command);

                ESP_LOGI(TAG, "Transmit command: %s", CmdObj->Command.c_str());

                // Transmit the command. The command has the layout
                //  Command<CR><LF>
                SIM70XX_UART_SendCommand(Device->UART, CmdObj->Command);

                // Flush the string, because the data aren´t needed anymore.
                CmdObj->Command.clear();

                // Destroy the command.
                delete CmdObj;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        // No messages pending. Get the asynchronous messages.
        if(Messages == 0)
        {
            // New data available?
            if(SIM70XX_UART_Available(Device->UART) > 2)
            {
                std::string* Message = new std::string();

                // Get the response string.
                do
                {
                    Message->append(SIM70XX_UART_ReadString(Device->UART));
                } while(SIM70XX_UART_Available(Device->UART));

                ESP_LOGD(TAG, "Event: %s", Message->c_str());

                // Shutdown message was received.
                if(Message->find("NORMAL POWER DOWN") != std::string::npos)
                {
                    Device->Internal.isActive = false;

                    // TODO: Event?

                    delete Message;

                    ESP_LOGI(TAG, "Power down event!");
                }
                else if(Message->find("EXIT PSM") != std::string::npos)
                {
                    SIM70XX_PSM_EVENT(Device, Message, false);
                }
                else if(Message->find("ENTER PSM") != std::string::npos)
                {
                    SIM70XX_PSM_EVENT(Device, Message, true);
                }
                #ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
                    else if(Message->find("+CSOERR") != std::string::npos)
                    {
                        SIM70XX_TCP_DISCONNECT_EVENT(Device, Message);
                    }
                #endif
                #ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
                    else if(Message->find("+CMQPUB") != std::string::npos)
                    {
                        SIM70XX_MQTT_PUB_EVENT(Device, Message);
                    }
                    // Handle MQTT(S) socket disconnect events.
                    else if(Message->find("+CMQDISCON") != std::string::npos)
                    {
                        SIM70XX_MQTT_DISCONNECT_EVENT(Device, Message);
                    }
                #endif
                #ifdef CONFIG_SIM70XX_DRIVER_WITH_HTTP
                    else if(Message->find("+CHTTPERR") != std::string::npos)
                    {
                        SIM70XX_HTTP_EVENT(Device, Message);
                    }
                #endif
                // Handle all other messages by putting them into the event queue.
                else
                {
                    xQueueSend(Device->Internal.EventQueue, &Message, 0);
                }
            }
        }

        // All messages have been send. Now we need to handle the responses. We expect a response for each command.
        std::list<SIM70XX_CmdResp_t*>::iterator it = ActiveCommands.begin();
        while(it != ActiveCommands.end())
        {
            uint32_t Now;

            ESP_LOGD(TAG, "Active commands: %u", ActiveCommands.size());

            SIM70XX_UART_ReadStringUntil(Device->UART, '\n');

            // Get the data from the command. The transmission has the following layout
            //  <CR><LF>+<Command>: <Data><CR><LF>
            if((*it)->recData)
            {
                // Wait for the device response and process the response.
                Now = SIM70XX_Tools_GetmsTimer();
                do
                {
                    size_t Index;
                    std::string Line;

                    // Read a new line from the serial interface.
                    // NOTE: The trailing \n is removed from the response!
                    Line = std::string(SIM70XX_UART_ReadStringUntil(Device->UART, '\n'));

                    // Remove the line ending.
                    Index = Line.find("\r");
                    if(Index != std::string::npos)
                    {
                        Line.replace(Index, std::string("\r").size(), "");
                    }

                    if(Line.length() > 0)
                    {
                        ESP_LOGI(TAG, "     Device response: %s", Line.c_str());
                    }

                    // The device has reported an error.
                    if(Line.find("ERROR") != std::string::npos)
                    {
                        ESP_LOGE(TAG, "     Device response error!");

                        (*it)->isError = true;

                        break;
                    }
                    // Filter out empty lines.
                    else if(Line.length() > 0)
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

                        // Add the received line to the response when the number of lines is 0.
                        if((*it)->Lines > 1)
                        {
                            (*it)->Response += Line + '\n';
                            (*it)->Lines--;
                        }
                        // Abort when all lines are received.
                        else
                        {
                            (*it)->Response = Line;

                            break;
                        }
                    }
                    // Abort when an empty line was received.
                    else if((Line.length() == 0) && ((*it)->Lines == 0))
                    {
                        break;
                    }
                    // Abort when a timeout occurs.
                    else if((SIM70XX_Tools_GetmsTimer() - Now) > (*it)->Timeout)
                    {
                        ESP_LOGE(TAG, "     Device response timout!");

                        (*it)->isTimeout = true;

                        break;
                    }

                    vTaskDelay(20 / portTICK_PERIOD_MS);
                } while(true);
            }

            // The device has not reported an error and no receive timeout was detected. We have to receive the status message now.
            if(((*it)->isError == false) && ((*it)->isTimeout == false))
            {
                // Wait for the status code with the layout
                //  <CR><LF>Status<CR><LF>
                Now = SIM70XX_Tools_GetmsTimer();
                do
                {
                    // Read a new line from the serial interface.
                    // NOTE: The trailing \n is removed from the response!
                    (*it)->Status.append(SIM70XX_UART_ReadStringUntil(Device->UART, '\n'));

                    // Abort when a timeout occurs.
                    if((SIM70XX_Tools_GetmsTimer() - Now) > (*it)->Timeout)
                    {
                        ESP_LOGE(TAG, "     Device status timout!");

                        (*it)->isTimeout = true;

                        break;
                    }

                    vTaskDelay(20 / portTICK_PERIOD_MS);
                } while((*it)->Status.length() < 2);

                // Remove the line endings.
                (*it)->Status.erase(std::remove((*it)->Status.begin(), (*it)->Status.end(), '\r'), (*it)->Status.end());
                (*it)->Status.erase(std::remove((*it)->Status.begin(), (*it)->Status.end(), '\n'), (*it)->Status.end());

                // Transmission is without error when 'OK' as status code is transmitted and when no event data are received.
                if((*it)->Status.find("OK") == std::string::npos)
                {
                    ESP_LOGE(TAG, "     Device status error!");

                    (*it)->isError = true;
                }

                ESP_LOGI(TAG, "     Device Status: %s", (*it)->Status.c_str());
            }

            // The command was processed completly. Push it to the response queue and remove it from the command list.
            xQueueSend(Device->Internal.RxQueue, &(*it), 0);
            ActiveCommands.erase(it++);
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

SIM70XX_Error_t SIM70XX_Evt_StartTask(TaskHandle_t* p_Handle, void* p_Arg)
{
    if((p_Handle == NULL) || (p_Arg == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

	#ifdef CONFIG_SIM7020_TASK_CORE_AFFINITY
        xTaskCreatePinnedToCore(SIM70XX_Evt_Task, "SIM70XX_Evt", SIM70XX_TASK_COM_STACK, p_Arg, CONFIG_SIM70XX_TASK_COM_PRIO, p_Handle, CONFIG_SIM70XX_TASK_COM_CORE);
	#else
        xTaskCreate(SIM70XX_Evt_Task, "SIM70XX_Evt", CONFIG_SIM70XX_TASK_COM_STACK, p_Arg, CONFIG_SIM70XX_TASK_COM_PRIO, p_Handle);
	#endif
    if(p_Handle == NULL)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    return SIM70XX_ERR_OK;
}