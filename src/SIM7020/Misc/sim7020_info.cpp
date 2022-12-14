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

#include "sim7020.h"

#include "../../Core/Queue/sim70xx_queue.h"
#include "../../Core/Commands/sim70xx_commands.h"

#include "../../Core/Arch/ESP32/Logging/sim70xx_logging.h"

static const char* TAG = "SIM7020_Info";

void SIM7020_Info_Print(SIM7020_Info_t* const p_Info)
{
    SIM70XX_LOGI(TAG, "Device information:");
    SIM70XX_LOGI(TAG, "     Manufacturer: %s", p_Info->Manufacturer.c_str());
    SIM70XX_LOGI(TAG, "     Firmware: %s", p_Info->Firmware.c_str());
    SIM70XX_LOGI(TAG, "     Model: %s", p_Info->Model.c_str());
    SIM70XX_LOGI(TAG, "     IMEI: %s",p_Info->IMEI.c_str());
    SIM70XX_LOGI(TAG, "     ICCID: %s", p_Info->ICCID.c_str());
}

SIM70XX_Error_t SIM7020_Info_GetDeviceInformation(SIM7020_t& p_Device, SIM7020_Info_t* const p_Info)
{
    if(p_Info == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
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

SIM70XX_Error_t SIM7020_Info_GetManufacturer(SIM7020_t& p_Device, std::string* const p_Manufacturer)
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
    *Command = SIM7020_AT_CGMI;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Manufacturer);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetModel(SIM7020_t& p_Device, std::string* const p_Model)
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
    *Command = SIM7020_AT_CGMM;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Model);
    }

    return SIM70XX_ERR_FAIL;  
}

SIM70XX_Error_t SIM7020_Info_GetFW(SIM7020_t& p_Device, std::string* const p_Firmware)
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
    *Command = SIM7020_AT_CGMR;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_Firmware);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetIMEI(SIM7020_t& p_Device, std::string* const p_IMEI)
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
    *Command = SIM7020_AT_CGSN;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_IMEI);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetICCID(SIM7020_t& p_Device, std::string* const p_ICCID)
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
    *Command = SIM7020_AT_CCID;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout))
    {
        return SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, p_ICCID);
    }

    return SIM70XX_ERR_FAIL;
}

SIM70XX_Error_t SIM7020_Info_GetNetworkRegistrationStatus(SIM7020_t& p_Device)
{
    std::string Response;
    SIM70XX_TxCmd_t* Command;

    if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CGREG;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    p_Device.Connection.Status = static_cast<SIM7020_NetReg_t>(SIM70XX_Tools_StringToUnsigned(Response.substr(Response.find(",") + 1)));

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Info_GetQuality(SIM7020_t& p_Device, SIM70XX_Qual_t* p_Report)
{
    int8_t RSSI;
    uint8_t RXQual;
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
    *Command = SIM7020_AT_CSQ;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    RSSI = static_cast<int8_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));
    RXQual = static_cast<uint8_t>(SIM70XX_Tools_StringToUnsigned(Response));

    if(RSSI == 0)
    {
        p_Report->RSSI = -110;
    }
    else if(RSSI == 1)
    {
        p_Report->RSSI= -108;
    }
    else if(RSSI == 2)
    {
        p_Report->RSSI= -106;
    }
    // Simple approach to convert the return value from the modem into a RSSI value.
    else if((RSSI >= 3) || (RSSI <= 30))
    {
        p_Report->RSSI= -105 + (2 * (RSSI - 3));
    }
    else
    {
        p_Report->RSSI= 0;
    }

    p_Report->RXQual = RXQual;

    if((p_Report->RSSI== 0) && (p_Report->RXQual == 0))
    {
        return SIM70XX_ERR_FAIL;
    }

    return SIM70XX_ERR_OK;
}

SIM70XX_Error_t SIM7020_Info_GetNetworkStatus(SIM7020_t& p_Device, SIM7020_NetState_t* const p_Status)
{
    size_t Index;
    std::string Response;
    std::string Dummy;
    SIM70XX_TxCmd_t* Command;

    if(p_Status == NULL)
    {
        return SIM70XX_ERR_INVALID_ARG;
    }
    else if(p_Device.Internal.isInitialized == false)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    SIM70XX_CREATE_CMD(Command);
    *Command = SIM7020_AT_CENG_R;
    SIM70XX_PUSH_QUEUE(p_Device.Internal.TxQueue, Command);
    if(SIM70XX_Queue_Wait(p_Device.Internal.RxQueue, Command->Timeout) == false)
    {
        return SIM70XX_ERR_FAIL;
    }
    SIM70XX_ERROR_CHECK(SIM70XX_Queue_PopItem(p_Device.Internal.RxQueue, &Response));

    Index = Response.find(",");
    if(Index == std::string::npos)
    {
        return SIM70XX_ERR_FAIL;
    }

    p_Status->sc_earfcn = SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response));
    p_Status->sc_earfcn_offset = static_cast<int8_t>(SIM70XX_Tools_StringToSigned(SIM70XX_Tools_SubstringSplitErase(&Response)));
    p_Status->sc_pci = static_cast<uint16_t>(SIM70XX_Tools_StringToUnsigned(SIM70XX_Tools_SubstringSplitErase(&Response)));

    p_Status->sc_cellid = SIM70XX_Tools_SubstringSplitErase(&Response);
    p_Status->sc_cellid.erase(std::remove(p_Status->sc_cellid.begin(), p_Status->sc_cellid.end(), '\"'), p_Status->sc_cellid.end());

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_rsrp = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_rsrq = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_rssi = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_snr = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_band = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_tac = Dummy;
        p_Status->sc_tac.erase(std::remove(p_Status->sc_tac.begin(), p_Status->sc_tac.end(), '\"'), p_Status->sc_tac.end());
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_ecl = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_tx_pwr = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    Dummy = SIM70XX_Tools_SubstringSplitErase(&Response);
    if(Dummy.size())
    {
        p_Status->sc_re_rsrp = static_cast<int16_t>(SIM70XX_Tools_StringToSigned(Dummy));
    }

    SIM70XX_LOGD(TAG, "sc_earfcn: %u", p_Status->sc_earfcn);
    SIM70XX_LOGD(TAG, "sc_earfcn_offset: %i", p_Status->sc_earfcn_offset);
    SIM70XX_LOGD(TAG, "sc_pci: %i", p_Status->sc_pci);
    SIM70XX_LOGD(TAG, "sc_cellid: %s", p_Status->sc_cellid.c_str());
    SIM70XX_LOGD(TAG, "sc_rsrp: %i", p_Status->sc_rsrp);
    SIM70XX_LOGD(TAG, "sc_rsrq: %i", p_Status->sc_rsrq);
    SIM70XX_LOGD(TAG, "sc_rssi: %i", p_Status->sc_rssi);
    SIM70XX_LOGD(TAG, "sc_snr: %i", p_Status->sc_snr);
    SIM70XX_LOGD(TAG, "sc_band: %u", p_Status->sc_band);
    SIM70XX_LOGD(TAG, "sc_tac: %s", p_Status->sc_tac.c_str());
    SIM70XX_LOGD(TAG, "sc_ecl: %u", p_Status->sc_ecl);
    SIM70XX_LOGD(TAG, "sc_tx_pwr: %i", p_Status->sc_tx_pwr);
    SIM70XX_LOGD(TAG, "sc_re_rsrp: %i", p_Status->sc_re_rsrp);

    return SIM70XX_ERR_OK;
}

#endif