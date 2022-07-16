 /*
 * sim7020_info.cpp
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

#if(CONFIG_SIMXX_DEV == 7020)

#include <esp_log.h>

#include "sim7020.h"
#include "../../Private/Queue/sim70xx_queue.h"
#include "../../Private/Commands/sim7020_commands.h"

static const char* TAG = "SIM7020_Info";

void SIM7020_Info_Print(const SIM7020_Info_t* p_Info)
{
    ESP_LOGI(TAG, "Device information:");
    ESP_LOGI(TAG, "     Manufacturer: %s", p_Info->Manufacturer.c_str());
    ESP_LOGI(TAG, "     Firmware: %s", p_Info->Firmware.c_str());
    ESP_LOGI(TAG, "     Model: %s", p_Info->Model.c_str());
    ESP_LOGI(TAG, "     IMEI: %s",p_Info->IMEI.c_str());
    ESP_LOGI(TAG, "     ICCID: %s", p_Info->ICCID.c_str());
}

SIM70XX_Error_t SIM7020_Info_GetDeviceInformation(const SIM7020_t* const p_Device, SIM7020_Info_t* p_Info)
{
    if((p_Device == NULL) || (p_Info == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_ERROR_CHECK(SIM7020_Info_GetManufacturer(p_Device, &p_Info->Manufacturer));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetModel(p_Device, &p_Info->Model));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetFW(p_Device, &p_Info->Firmware));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetIMEI(p_Device, &p_Info->IMEI));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetICCID(p_Device, &p_Info->ICCID));
    SIM70XX_ERROR_CHECK(SIM7020_Info_GetFW(p_Device, &p_Info->Firmware));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Info_GetManufacturer(const SIM7020_t* const p_Device, std::string* p_Manufacturer)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Manufacturer == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGMI;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, p_Manufacturer);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetModel(const SIM7020_t* const p_Device, std::string* p_Model)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Model == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGMM;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, p_Model);
    }

    return SIM70XX_ERR_FAIL;  
}

SIM70XX_Error_t SIM7020_Info_GetFW(const SIM7020_t* const p_Device, std::string* p_Firmware)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Firmware == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGMR;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, p_Firmware);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetIMEI(const SIM7020_t* const p_Device, std::string* p_IMEI)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_IMEI == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGSN;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, p_IMEI);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetICCID(const SIM7020_t* const p_Device, std::string* p_ICCID)
{
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_ICCID == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CCID;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, p_ICCID);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetNetworkRegistrationStatus(SIM7020_t* const p_Device)
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
    *Command = SIM7020_AT_CGREG;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    p_Device->Connection.Status = (SIM7020_NetRegistration_t)std::stoi(Response.substr(Response.find(",") + 1));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Info_GetQuality(SIM7020_t* const p_Device)
{
    int8_t RSSI;
    uint8_t RXQual;
    size_t Index;
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
    *Command = SIM7020_AT_CSQ;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    Index = Response.find(",");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    RSSI = std::stoi(Response.substr(0, Index));
    RXQual = std::stoi(Response.substr(Response.find_last_of(",") + 1));

    if(RSSI == 0)
    {
        p_Device->Connection.RSSI = -110;
    }
    else if(RSSI == 1)
    {
        p_Device->Connection.RSSI = -108;
    }
    else if(RSSI == 2)
    {
        p_Device->Connection.RSSI = -106;
    }
    // Simple approach to convert the return value from the modem into a RSSI value.
    else if((RSSI >= 3) || (RSSI <= 30))
    {
        p_Device->Connection.RSSI = -105 + (2 * (RSSI - 3));
    }
    else
    {
        p_Device->Connection.RSSI = 0;
    }

    p_Device->Connection.RXQual = RXQual;

    if((RSSI == 0) && (RXQual == 0))
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Info_GetNetworkStatus(const SIM7020_t* const p_Device, SIM7020_NetState_t* p_Status)
{
    size_t Index;
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if((p_Device == NULL) || (p_Status == NULL))
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device->Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CENG_R;
    SIM70XX_PUSH_QUEUE(p_Device->Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device->Internal.RxQueue, &p_Device->Internal.isActive, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device->Internal.RxQueue, &Response));

    Index = Response.find(",");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Status->sc_earfcn = std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_earfcn_offset = (int8_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_pci = std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_cellid = Response.substr(0, Index);
    p_Status->sc_cellid.erase(std::remove(p_Status->sc_cellid.begin(), p_Status->sc_cellid.end(), '"'), p_Status->sc_cellid.end());
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_rsrp = (int16_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_rsrq = (int16_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_rssi = (int16_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_snr = (int16_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_band = std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_tac = Response.substr(0, Index);
    p_Status->sc_tac.erase(std::remove(p_Status->sc_tac.begin(), p_Status->sc_tac.end(), '"'), p_Status->sc_tac.end());
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_ecl = std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    Index = Response.find(",");
    p_Status->sc_tx_pwr = (int16_t)std::stoi(Response.substr(0, Index));
    Response.erase(0, Index + 1);

    p_Status->sc_re_rsrp = (int16_t)std::stoi(Response);

    ESP_LOGI(TAG, "sc_earfcn: %u", p_Status->sc_earfcn);
    ESP_LOGI(TAG, "sc_earfcn_offset: %i", p_Status->sc_earfcn_offset);
    ESP_LOGI(TAG, "sc_pci: %i", p_Status->sc_pci);
    ESP_LOGI(TAG, "sc_cellid: %s", p_Status->sc_cellid.c_str());
    ESP_LOGI(TAG, "sc_rsrp: %i", p_Status->sc_rsrp);
    ESP_LOGI(TAG, "sc_rsrq: %i", p_Status->sc_rsrq);
    ESP_LOGI(TAG, "sc_rssi: %i", p_Status->sc_rssi);
    ESP_LOGI(TAG, "sc_snr: %i", p_Status->sc_snr);
    ESP_LOGI(TAG, "sc_band: %u", p_Status->sc_band);
    ESP_LOGI(TAG, "sc_tac: %s", p_Status->sc_tac.c_str());
    ESP_LOGI(TAG, "sc_ecl: %u", p_Status->sc_ecl);
    ESP_LOGI(TAG, "sc_tx_pwr: %i", p_Status->sc_tx_pwr);
    ESP_LOGI(TAG, "sc_re_rsrp: %i", p_Status->sc_re_rsrp);

    return SIM70XX_ERR_OK;
}

#endif