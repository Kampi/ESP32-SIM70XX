 /*
 * sim7080_info.cpp
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
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim70xx_commands.h"

static const char* TAG = "SIM7080_Info";

void SIM7080_Info_Print(SIM7080_Info_t* const p_Info)
{
    ESP_LOGI(TAG, "Device information:");
    ESP_LOGI(TAG, "     Manufacturer: %s", p_Info->Manufacturer.c_str());
    ESP_LOGI(TAG, "     Firmware: %s", p_Info->Firmware.c_str());
    ESP_LOGI(TAG, "     Model: %s", p_Info->Model.c_str());
    ESP_LOGI(TAG, "     IMEI: %s",p_Info->IMEI.c_str());
    ESP_LOGI(TAG, "     ICCID: %s", p_Info->ICCID.c_str());
}

SIM70XX_Error_t SIM7080_Info_GetDeviceInformation(SIM7080_t& p_Device, SIM7080_Info_t* const p_Info)
{
    if(p_Info == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7080_Info_GetManufacturer(p_Device, &p_Info->Manufacturer));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetModel(p_Device, &p_Info->Model));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetFW(p_Device, &p_Info->Firmware));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetIMEI(p_Device, &p_Info->IMEI));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetICCID(p_Device, &p_Info->ICCID));
    SIM70XX_ERROR_CHECK(SIM7080_Info_GetFW(p_Device, &p_Info->Firmware));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Info_GetManufacturer(SIM7080_t& p_Device, std::string* const p_Manufacturer)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Manufacturer == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGMI;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Manufacturer);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Info_GetModel(SIM7080_t& p_Device, std::string* const p_Model)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Model == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGMM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Model);
    }

    return SIM70XX_ERR_FAIL;  
}

SIM70XX_Error_t SIM7080_Info_GetFW(SIM7080_t& p_Device, std::string* const p_Firmware)
{
    SIM70XX_TxCmd_t* Command;

    if(p_Firmware == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGMR;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Firmware);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Info_GetIMEI(SIM7080_t& p_Device, std::string* const p_IMEI)
{
    SIM70XX_TxCmd_t* Command;

    if(p_IMEI == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGSN;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_IMEI);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Info_GetICCID(SIM7080_t& p_Device, std::string* const p_ICCID)
{
    SIM70XX_TxCmd_t* Command;

    if(p_ICCID == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CCID;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_ICCID);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7080_Info_GetNetworkRegistrationStatus(SIM7080_t& p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CGREG;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.Connection.Status = (SIM7080_NetRegistration_t)std::stoi(Response.substr(Response.find(",") + 1));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Info_GetQuality(SIM7080_t& p_Device, SIM70XX_Qual_t* p_Report)
{
    int8_t RSSI;
    uint8_t RXQual;
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }
    else if(p_Report == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CSQ;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    // TODO: Implement me
    Index = Response.find(",");
    RSSI = std::stoi(Response.substr(0, Index));
    RXQual = std::stoi(Response.substr(Response.find_last_of(",") + 1));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7080_Info_GetEquipmentInfo(SIM7080_t& p_Device, SIM7080_UEInfo_t* p_Info)
{
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

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7080_AT_CPSI;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, &p_Device.Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    if(Response.find("NO SERVICE,Online") != std::string::npos)
    {
        return SIM70XX_ERR_NOT_READY;
    }

    p_Info->SystemMode = SIM70XX_Tools_SubstringSplitErase(&Response);
    p_Info->OperationMode = SIM70XX_Tools_SubstringSplitErase(&Response);
    p_Info->MCC = SIM70XX_Tools_SubstringSplitErase(&Response);
    p_Info->MNC = SIM70XX_Tools_SubstringSplitErase(&Response);

    if(p_Info->SystemMode == "GSM")
    {
        p_Info->GSM.LAC = SIM70XX_Tools_SubstringSplitErase(&Response);
        // TODO
    }
    else
    {
        p_Info->CAT.TAC = SIM70XX_Tools_SubstringSplitErase(&Response);
        p_Info->CAT.SCellID = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.Band = SIM70XX_Tools_SubstringSplitErase(&Response);
        p_Info->CAT.earfcn = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.dlbw = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.ulbw = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.RSRQ = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.RSRP = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.RSSI = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
        p_Info->CAT.RSSNR = std::stoi(SIM70XX_Tools_SubstringSplitErase(&Response));
    }

    return SIM70XX_ERR_OK;
}

#endif