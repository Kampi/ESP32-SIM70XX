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

#if(CONFIG_SIMXX_DEV == 7080)

#include <esp_log.h>

#include "sim7080.h"
#include "../Private/UART/sim70xx_uart.h"
#include "../Private/Events/sim70xx_evt.h"
#include "../Private/Queue/sim70xx_queue.h"
#include "../Private/Commands/sim7080_commands.h"

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
        SIM7080_Deinit(p_Device);
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
    SIM70XX_ERROR_CHECK(SIM70XX_Evt_StartTask(&p_Device->Internal.TaskHandle, p_Device));

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
        SIM70XX_ERROR_CHECK(SIM7080_SoftReset(p_Device, Timeout));
	#endif

    vTaskSuspend(p_Device->Internal.TaskHandle);
    SIM70XX_DisableEcho(&p_Device->UART);
    vTaskResume(p_Device->Internal.TaskHandle);

    SIM70XX_ERROR_CHECK(SIM7080_Ping(p_Device));

    if(SIM7080_isSIMReady(p_Device) == false)
    {
        ESP_LOGE(TAG, "SIM not ready!");
    }
    ESP_LOGI(TAG, "SIM ready!");

    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_MIN));
    SIM70XX_ERROR_CHECK(SIM7080_SetMode(p_Device, p_Config->Mode));
    SIM70XX_ERROR_CHECK(SIM7080_SetSelection(p_Device, p_Config->Selection));
    SIM70XX_ERROR_CHECK(SIM7080_SetOperator(p_Device, SIM_MODE_MANUAL, p_Config->OperatorFormat, p_Config->Operator));
    SIM70XX_ERROR_CHECK(SIM7080_SetFunctionality(p_Device, SIM7080_FUNC_FULL));
    SIM70XX_ERROR_CHECK(SIM7080_SetBand(p_Device, p_Config->Band));

    do
    {
        ESP_LOGI(TAG, "Waiting...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while(SIM7080_isAttached(p_Device) == false);
    ESP_LOGI(TAG, "Connected...");

    SIM70XX_ERROR_CHECK(SIM7080_Info_GetNetworkRegistrationStatus(p_Device));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetQuality(p_Device));
    //SIM70XX_ERROR_CHECK(SIM7020_PDP_ReadDynamicParameters(p_Device));
    //SIM70XX_ERROR_CHECK(SIM7020_PSM_GetEventStatus(p_Device, &p_Device->Internal.isPSMEvent));

/*
    SIM7020_PSM_Enable(p_Device, 3, 15, 0, 15);
    SIM70XX_ERROR_CHECK(SIM7020_PSM_SetEventStatus(p_Device, true));
    Error = SIM7020_GetOperator(p_Device, &p_Device->Operators, &p_Device->Modes, &p_Device->Formats);
*/

    return SIM70XX_ERR_OK;
}

void SIM7080_Deinit(SIM7080_t* const p_Device)
{
    assert(p_Device);

    // Stop the receive task.
    vTaskSuspend(p_Device->Internal.TaskHandle);
    vTaskDelete(p_Device->Internal.TaskHandle);
    p_Device->Internal.TaskHandle = NULL;

    // Delete the queues.
    vQueueDelete(p_Device->Internal.RxQueue);
    vQueueDelete(p_Device->Internal.TxQueue);
    vQueueDelete(p_Device->Internal.EventQueue);

    // TODO: Shutdown modem

    // Deinitialize the modem.
    SIM70XX_UART_Deinit(&p_Device->UART);
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

SIM70XX_Error_t SIM7080_SoftReset(const SIM7080_t* const p_Device, uint32_t Timeout)
{
    uint32_t Now;
    std::string Response;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    if(p_Device->Internal.TaskHandle != NULL)
    {
        vTaskSuspend(p_Device->Internal.TaskHandle);
    }

    ESP_LOGI(TAG, "Performing soft reset...");
    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        // Reset the module.
        // NOTE: Echo mode is enabled after a reset!
        SIM70XX_UART_SendCommand(&p_Device->UART, "ATZ");
        Response = SIM70XX_UART_ReadStringUntil(&p_Device->UART, '\n');
        Response = SIM70XX_UART_ReadStringUntil(&p_Device->UART, '\n');
        ESP_LOGI(TAG, "Response: %s", Response.c_str());

        // Check if the reset was successful.
        if(Response.find("OK") != std::string::npos)
        {
            ESP_LOGI(TAG, "     Software reset successful!");

            if(p_Device->Internal.TaskHandle != NULL)
            {
                vTaskResume(p_Device->Internal.TaskHandle);
            }

            return SIM70XX_ERR_OK;
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    } while((SIM70XX_Tools_GetmsTimer() - Now) < (Timeout * 1000UL));

    ESP_LOGE(TAG, "     Software reset timeout!");

    if(p_Device->Internal.TaskHandle != NULL)
    {
        vTaskResume(p_Device->Internal.TaskHandle);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_SetOperator(SIM7080_t* const p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator, SIM7080_AcT_t AcT)
{
    std::string CommandStr;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    CommandStr = "AT+COPS=" + std::to_string(Mode);

    if((Mode == SIM_MODE_MANUAL) || (Mode == SIM_MODE_BOTH))
    {
        CommandStr += "," + std::to_string(Format) + ",\"" + Operator + "\"" + "," + std::to_string(AcT);
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_COPS_W(CommandStr);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, NULL, NULL);
}

SIM70XX_Error_t SIM7080_SetBand(SIM7080_t* const p_Device, SIM7080_Band_t Band)
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
    *Command = SIM70XX_AT_CBAND_W(Band);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetBand(SIM7080_t* const p_Device, SIM7080_Band_t* p_Band)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Band == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == SIM7080_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CBAND_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    if(Response.find("EGSM_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_EGSM;
    }
    else if(Response.find("DCS_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_DCS;
    }
    else if(Response.find("ALL_MODE") != std::string::npos)
    {
        *p_Band = SIM7080_BAND_ALL;
    }
    else
    {
        *p_Band = (SIM7080_Band_t)std::stoi(Response);
    }

    ESP_LOGD(TAG, "Band: %u", *p_Band);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetMode(SIM7080_t* const p_Device, SIM7080_NetMode_t Mode)
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
    *Command = SIM7080_AT_CNMP_W(Mode);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetMode(SIM7080_t* const p_Device, SIM7080_NetMode_t* p_Mode)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Mode == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == SIM7080_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CNMP_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    *p_Mode = (SIM7080_NetMode_t)std::stoi(Response);

    ESP_LOGD(TAG, "Mode: %u", *p_Mode);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetSelection(SIM7080_t* const p_Device, SIM7080_Sel_t Selection)
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
    *Command = SIM7080_AT_CMNB_W(Selection);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetSelection(SIM7080_t* const p_Device, SIM7080_Sel_t* p_Selection)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Selection == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == SIM7080_FUNC_MIN)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CMNB_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    *p_Selection = (SIM7080_Sel_t)std::stoi(Response);

    ESP_LOGD(TAG, "Selection: %u", *p_Selection);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_SetFunctionality(SIM7080_t* const p_Device, SIM7080_Func_t Func)
{
    std::string Status;
    std::string Response;
    SIM70XX_TxCmd_t* Command;
    SIM70XX_Error_t Error = SIM70XX_ERR_FAIL;

    if(p_Device == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device->Connection.Functionality == Func)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CFUN_W(Func);
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }

    // NOTE: Do not use the error macro, because the response and status depends on the current state of the device.
    Error = SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response, &Status);

    // Device is in minimum functionality -> Transition into another functionality
    //  Response = OK
    //  Status = +CPIN: READY
    if(p_Device->Connection.Functionality == SIM7080_FUNC_MIN)
    {
        ESP_LOGI(TAG, "Minimum functionality...");

        if(Response.find("OK") != std::string::npos)
        {
            p_Device->Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }
    }
    // Device is in full functionality -> Transition into another functionality
    //  Response = +CPIN: NOT READY
    //  Status = OK
    else if(p_Device->Connection.Functionality == SIM7080_FUNC_FULL)
    {
        ESP_LOGI(TAG, "Full functionality...");

        if(Status.find("OK") != std::string::npos)
        {
            p_Device->Connection.Functionality = Func;

            Error = SIM70XX_ERR_OK;
        }
    }

    ESP_LOGI(TAG, "Set functionality: %u", p_Device->Connection.Functionality);

    return Error;
}

SIM70XX_Error_t SIM7080_GetFunctionality(SIM7080_t* const p_Device)
{
    std::string Response;
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
    *Command = SIM70XX_AT_CFUN_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Device->Connection.Functionality = (SIM7080_Func_t)std::stoi(Response);

    ESP_LOGD(TAG, "Functionality: %u", p_Device->Connection.Functionality);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GetSIMStatus(SIM7080_t* const p_Device, SIM7080_SIM_t* const p_Status)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Status == NULL))
    {
        return false;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return false;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CPIN_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return false;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    if(Response.find("READY") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_READY;
    }
    else if(Response.find("SIM PIN") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PIN;
    }
    else if(Response.find("SIM PUK") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PUK;
    }
    else if(Response.find("PH_SIM PIN") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PH_PIN;
    }
    else if(Response.find("PH_SIM PUK") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PH_PUK;
    }
    else if(Response.find("PH_SIM NET") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_NET_PIN;
    }
    else if(Response.find("SIM PIN2") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PIN2;
    }
    else if(Response.find("SIM PUK2") != std::string::npos)
    {
        *p_Status = SIM7080_SIM_WAIT_PUK2;
    }
    else
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

bool SIM7080_isSIMReady(SIM7080_t* const p_Device)
{
    SIM7080_SIM_t Status;

    if(SIM7080_GetSIMStatus(p_Device, &Status) != SIM70XX_ERR_OK)
    {
        return false;
    }

    if(Status == SIM7080_SIM_READY)
    {
        return true;
    }

    return false;   
}

bool SIM7080_isAttached(SIM7080_t* const p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device == NULL)
    {
        return false;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return false;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM70XX_AT_CGATT_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return false;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    return (bool)std::stoi(Response);
}

bool SIM7080_isInitialized(SIM7080_t* const p_Device)
{
    if(p_Device == NULL)
    {
        return false;
    }

    return p_Device->Internal.isInitialized;
}

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
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
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