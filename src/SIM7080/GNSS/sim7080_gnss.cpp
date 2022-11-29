 /*
 * sim7080_gnss.cpp
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

#if((CONFIG_SIMXX_DEV == 7080) && (defined CONFIG_SIM70XX_DRIVER_WITH_GNSS))

#include "sim7080.h"
#include "sim7080_gnss.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Timer/sim70xx_timer.h"
#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7080_GNSS";

SIM70XX_Error_t SIM7080_GNSS_Config(SIM7080_t& p_Device, const SIM7080_GNSS_Config_t* const p_Config)
{
    uint16_t Threshold;
    uint32_t Timeout;
    SIM70XX_TxCmd_t* Command;

    if((p_Config == NULL) || (p_Config->Port != SIM7080_GNSS_PORT_OFF) ||
       ((p_Config->Threshold > 0) && (p_Config->Threshold > 10000)) ||
       ((p_Config->Timeout > 0) && ((p_Config->Timeout < 10000) || (p_Config->Timeout > 180000)))
       )
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    Threshold = p_Config->Threshold;
    if(p_Config->Threshold == 0)
    {
        Threshold = 10000;
    }

    Timeout = p_Config->Timeout;
    if(p_Config->Timeout == 0)
    {
        Timeout = 30000;
    }

    // TODO: Add support for GPS output ports.

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("NMEATYPE", p_Config->NMEA);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("OUTURC", p_Config->EnableURC);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("ADSS", p_Config->ADSS);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("MODE", p_Config->Network);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("THRESHOLD", Threshold);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("TIMEOUT", Timeout);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCFG("EXTRAINFO", p_Config->Flag);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GNSS_ColdStart(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGNSCOLD;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GNSS_WarmStart(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGNSWARM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GNSS_HotStart(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGNSHOT;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GNSS_Enable(SIM7080_t& p_Device, bool Enable)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGNSPWR_W(Enable);
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue);
}

SIM70XX_Error_t SIM7080_GNSS_isEnabled(SIM7080_t& p_Device, bool* p_Enable)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Enable == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGNSPWR_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    *p_Enable = static_cast<bool>(SIM70XX_Tools_StringToUnsigned(Response));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GNSS_GetSingleLocation(SIM7080_t& p_Device, SIM7080_GNSS_Data_t* p_Data, SIM7080_GNSS_PwrLevel_t Power, SIM7080_GNSS_Error_t* p_Error)
{
    bool Enabled;
    SIM70XX_TxCmd_t* Command;
    SIM7080_GNSS_Error_t Error;

    if(p_Data == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Check if GPS is enabled.
    SIM70XX_ERROR_CHECK(SIM7080_GNSS_isEnabled(p_Device, &Enabled));
    if(Enabled)
    {
        return SIM70XX_ERR_INVALID_STATE;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCMD(std::string("AT+SGNSCMD=" + std::to_string(SIM7080_GNSS_MODE_ON_SINGLE) + "," + std::to_string(Power)));
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    Error = SIM7080_GNSS_ERR_OK;
    while(true)
    {
        std::string Event;

        if(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "SGNSERR", &Event))
        {
            SIM70XX_LOGE(TAG, "GPS error");

            break;
        }
        else if(SIM70XX_Queue_isEvent(p_Device.Internal.EventQueue, "SGNSCMD", &Event))
        {
            // Remove the command.
            SIM70XX_Tools_SubstringSplitErase(&Event);

            p_Data->Time = SIM70XX_Tools_SubstringSplitErase(&Event);
            p_Data->Latitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->Longitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->Accuracy = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->Altitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->SeaLevel = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->Speed = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));
            p_Data->Course = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Event));

            break;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if(p_Error != NULL)
    {
        *p_Error = Error;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GNSS_GetNavInfo(SIM7080_t& p_Device, SIM7080_GNSS_Info_t* p_Info, uint32_t Timeout)
{
    bool Enabled;
    uint32_t Now;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Info == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Check if GPS is enabled.
    SIM70XX_ERROR_CHECK(SIM7080_GNSS_isEnabled(p_Device, &Enabled));
    if(Enabled == false)
    {
        SIM70XX_LOGE(TAG, "GPS not enabled!");

        return SIM70XX_ERR_INVALID_STATE;
    }

    Enabled = false;
    Now = SIM70XX_Timer_GetMilliseconds();
    do
    {
        SIM70XX_CREATE_CMD(Command);
        *Command = SIM7080_AT_CGNSINF;
        SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
        if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
        {
            return SIM70XX_ERR_NOT_READY;
        }
        SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

        // Check if GPS is on.
        if(static_cast<bool>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response))))
        {
            std::string Substring;

            SIM70XX_LOGI(TAG, "GPS enabled...");

            Substring = SIM70XX_Tools_SubstringSplitErase(&Response);
            if(Substring.size() > 0)
            {
                Enabled = static_cast<bool>(SIM70XX_Tools_StringToUnsigned(Substring));
            }
            else
            {
                Enabled = false;
            }

            // Check if GPS is locked.
            if(Enabled)
            {
                SIM70XX_LOGI(TAG, "GPS locked...");

                p_Info->DateTime = SIM70XX_Tools_SubstringSplitErase(&Response);
                p_Info->Latitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->Longitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->Altitude = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->Speed = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->Course = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));

                // Fix mode.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                // Reserved.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                p_Info->HDOP = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->PDOP = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->VDOP = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));

                // Reserved.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                p_Info->Satellites = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));

                // Reserved.
                SIM70XX_Tools_SubstringSplitErase(&Response);

                p_Info->HPA = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));
                p_Info->VPA = SIM70XX_Tools_StringToFloat(SIM70XX_Tools_SubstringSplitErase(&Response));

                break;
            }
            else
            {
                SIM70XX_LOGI(TAG, "GPS not locked...");
            }
        }
        else
        {
            SIM70XX_LOGI(TAG, "GPS not enabled...");
        }

        if((SIM70XX_Timer_GetMilliseconds() - Now) > (Timeout * 1000UL))
        {
            return SIM70XX_ERR_TIMEOUT;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while (true);

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GNSS_Start(SIM7080_t& p_Device, SIM7080_GNSS_Accuracy_t Accuracy, uint32_t minDistance, uint32_t minInterval)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCMD(std::string("AT+SGNSCMD=" + std::to_string(SIM7080_GNSS_MODE_ON_CONT) + "," + std::to_string(minInterval) + "," + std::to_string(minDistance) + "," + std::to_string(Accuracy)));
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Device.GNSS.isListening = true;

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_GNSS_Stop(SIM7080_t& p_Device)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Device.GNSS.isListening == false)
    {
        return SIM70XX_ERR_OK;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_SGNSCMD(std::string("AT+SGNSCMD=" + std::to_string(SIM7080_GNSS_MODE_OFF)));
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_NOT_READY;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue));

    p_Device.GNSS.isListening = false;

    return SIM70XX_ERR_OK;
}

#endif