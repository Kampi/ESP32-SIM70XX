 /*
 * sim7080.cpp
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

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_DEV_SIM7080

#include <esp_log.h>

#include "sim7080.h"
#include "../Private/UART/sim70xx_uart.h"
#include "../Private/Events/sim70xx_evt.h"
#include "../Private/Queue/sim70xx_queue.h"
#include "../Private/Commands/sim7080_commands.h"

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

static const char* TAG = "SIM7080";

SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, uint32_t Timeout)
{
    return SIM7080_Init(p_Device, p_Config, Timeout, SIM_BAUD_AUTO);
}

SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, SIM70XX_Baud_t Old)
{
    return SIM7080_Init(p_Device, p_Config, 10, Old);
}

SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, uint32_t Timeout, SIM70XX_Baud_t Old)
{
    std::string Response;

    if((p_Config == NULL) || (p_Device == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized)
    {
        //SIM7080_Deinit(p_Device);
    }

    p_Device->Internal.RxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_CmdResp_t*));
    p_Device->Internal.TxQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(SIM70XX_TxCmd_t*));
    p_Device->Internal.EventQueue = xQueueCreate(CONFIG_SIM70XX_QUEUE_LENGTH, sizeof(std::string*));
    if((p_Device->Internal.RxQueue == NULL) || (p_Device->Internal.TxQueue == NULL) || (p_Device->Internal.EventQueue == NULL))
    {
        return SIM70XX_ERR_NO_MEM;
    }

    p_Device->UART.Interface = p_Config->UART.Interface;
    p_Device->UART.Rx = p_Config->UART.Rx;
    p_Device->UART.Tx = p_Config->UART.Tx;
    p_Device->UART.Baudrate = p_Config->UART.Baudrate;

    SIM70XX_ERROR_CHECK(SIM70XX_UART_Init(&p_Device->UART));

    // Get all remaining available data to clear the Rx buffer.
    SIM70XX_UART_Flush(&p_Device->UART);

    // No receive task started yet. Start the receive task.
	#ifdef CONFIG_SIM7020_TASK_CORE_AFFINITY
        xTaskCreatePinnedToCore(SIM70XX_Evt_Task, "comTask", SIM70XX_TASK_COM_STACK, p_Device, CONFIG_SIM70XX_TASK_COM_PRIO, &p_Device->Internal.TaskHandle, CONFIG_SIM70XX_TASK_COM_CORE);
	#else
        xTaskCreate(SIM70XX_Evt_Task, "comTask", CONFIG_SIM70XX_TASK_COM_STACK, p_Device, CONFIG_SIM70XX_TASK_COM_PRIO, &p_Device->Internal.TaskHandle);
	#endif
    if(p_Device->Internal.TaskHandle == NULL)
    {
        return SIM70XX_ERR_NO_MEM;
    }

    p_Device->Internal.isActive = true;
    p_Device->Internal.isInitialized = true;

    if(Old != SIM_BAUD_AUTO)
    {
        /*
        SIM70XX_Error_t Error;
        ESP_LOGI(TAG, "Changing baudrate...");

        p_Device->Internal.isInitialized = true;
        Error = SIM7020_SetBaudrate(p_Device, Old, p_Config->UART.Baudrate);
        p_Device->Internal.isInitialized = false;
        if(Error != SIM70XX_ERR_OK)
        {
            return Error;
        }*/
    }

	#ifdef CONFIG_SIM70XX_RESET_USE_HW
        p_Device->Interface.Reset_Conf.Inverted = p_Config->Interface.Reset_Conf.Inverted;
        p_Device->Interface.Reset_Conf.Pin = p_Config->Interface.Reset_Conf.Pin;
		if(p_Device->Interface.Reset_Conf.Pin != GPIO_NUM_NC)
		{
		    gpio_set_direction(p_Device->Interface.Reset_Conf.Pin, GPIO_MODE_OUTPUT);
		    SIM7080_HardReset(p_Device);
		}
    #else
        //SIM70XX_ERROR_CHECK(SIM7020_SoftReset(p_Device, Timeout));
	#endif

    vTaskSuspend(p_Device->Internal.TaskHandle);
    SIM70XX_DisableEcho(&p_Device->UART);
    vTaskResume(p_Device->Internal.TaskHandle);

    // Check if the module is ready.
    SIM70XX_ERROR_CHECK(SIM7080_Ping(p_Device));
        /*SIM70XX_ERROR_CHECK(SIM7020_PSM_Disable(p_Device, SIM_PSM_DIS));
    if(SIM7020_GetFunctionality(p_Device) == SIM70XX_ERR_INVALID_STATE)
    {

    }
    SIM70XX_ERROR_CHECK(SIM7020_GetBand(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_SetPSD(p_Device, SIM_PDP_IP, p_Config->APN));
    SIM70XX_ERROR_CHECK(SIM7020_SetBand(p_Device, p_Config->Band));
    SIM70XX_ERROR_CHECK(SIM7020_SetOperator(p_Device, SIM_MODE_MANUAL, p_Config->OperatorFormat, p_Config->Operator));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetNetworkRegistrationStatus(p_Device));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetQuality(p_Device));*/
    //SIM70XX_ERROR_CHECK(SIM7020_PDP_ReadDynamicParameters(p_Device));
    //SIM70XX_ERROR_CHECK(SIM7020_PSM_GetEventStatus(p_Device, &p_Device->Internal.isPSMEvent));

/*
    SIM7020_PSM_Enable(p_Device, 3, 15, 0, 15);
    SIM70XX_ERROR_CHECK(SIM7020_PSM_SetEventStatus(p_Device, true));
    Error = SIM7020_GetOperator(p_Device, &p_Device->Operators, &p_Device->Modes, &p_Device->Formats);
*/

    return SIM70XX_ERR_OK;
}

#ifdef CONFIG_SIM70XX_RESET_USE_HW
	void SIM7080_HardReset(SIM7080_t* const p_Device )
	{
		assert(p_Device);

		if(p_Device->Interface.Reset_Conf.Pin != GPIO_NUM_NC)
		{
			if(p_Device->Interface.Reset_Conf.Inverted)
			{
				gpio_set_level(p_Device->Interface.Reset_Conf.Pin, true);
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				gpio_set_level(p_Device->Interface.Reset_Conf.Pin, false);
			}
		}
	}
#endif

SIM70XX_Error_t SIM7080_Ping(SIM7080_t* const p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    if(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue) != SIM70XX_ERR_OK)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    return SIM70XX_ERR_OK;
}

#endif