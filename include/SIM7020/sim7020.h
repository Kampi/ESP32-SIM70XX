 /*
 * sim7020.h
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

#ifndef SIM7020_H_
#define SIM7020_H_

#include <stdint.h>
#include <stdbool.h>

#include "sim7020_pdp.h"
#include "sim7020_info.h"
#include "sim7020_defs.h"
#include "sim7020_fota.h"
#include "sim70xx_tools.h"
#include "sim70xx_errors.h"
#include "sim7020_pwrmgnt.h"

#include "sim7020_config_1nce.h"
#include "sim7020_config_fusion.h"

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_DRIVER_WITH_NVRAM
    #include "sim7020_nvram.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
    #include "sim7020_tcpip.h"
    #include "sim7020_config_ping.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_DNS
    #include "sim7020_dns.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_NTP
    #include "sim7020_ntp.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_HTTP
    #include "sim7020_http.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
    #include "sim7020_mqtt.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_COAP
    #include "sim7020_coap.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_TLS
    #include "sim7020_tls.h"
#endif

/** @brief  Size of the command buffer of the SIM7020 module in bytes.
 *          NOTE: The leading "AT" from the first command doesn´t count to the buffer size!
 */
#define SIM7020_CMD_BUFFER                                      2048

/** @brief          Check if the module is initialized.
 *  @param p_Device SIM7020 device object
 *  @return         #true when the module is initialized
 */
inline __attribute__((always_inline)) bool SIM7020_isInitialized(SIM7020_t& p_Device)
{
    return p_Device.Internal.isInitialized;
}

/** @brief          Initialize the communication interface and the SIM7020 module.
 *  @param p_Device SIM7020 device object
 *  @param p_Config SIM7020 device configuration object
 *  @param Timeout  Reset timeout
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, uint32_t Timeout);

/** @brief          Initialize the communication interface and the SIM7020 module.
 *  @param p_Device SIM7020 device object
 *  @param p_Config SIM7020 device configuration object
 *  @param Old      Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, SIM70XX_Baud_t Old);

/** @brief          Initialize the communication interface and the SIM7020 module.
 *  @param p_Device SIM7020 device object
 *  @param p_Config SIM7020 device configuration object
 *  @param Timeout  (Optional) Reset timeout
 *  @param Old      (Optional) Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Init(SIM7020_t& p_Device, SIM7020_Config_t& p_Config, uint32_t Timeout = 10, SIM70XX_Baud_t Old = SIM_BAUD_AUTO);

/** @brief          Deinitialize the SIM7020 module.
 *  @param p_Device SIM7020 device object
 *  @param Skip     (Optional) Skip the shutdown command for the modem
 *                  NOTE: This is usefull when you have used \ref SIM70XX_Tools_DisableModule to shutdown the modem.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Deinit(SIM7020_t& p_Device, bool Skip = false);

/** @brief          Perform a software reset of the device.
 *  @param p_Device SIM7020 device object
 *  @param Timeout  (Optional) Timeout for the device reset in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_SoftReset(SIM7020_t& p_Device, uint32_t Timeout = 10);

/** @brief          Set the default PSD connection settings.
 *  @param p_Device SIM7020 device object
 *  @param PDP      Packet Data Protocol (PDP) type
 *  @param APN      Access Point Name (APN) configuration
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_SetPSD(SIM7020_t& p_Device, SIM7020_PDP_Type_t PDP, SIM70XX_APN_t APN);

/** @brief          Set the device operator.
 *  @param p_Device SIM7020 device object
 *  @param Mode     Operator selection mode
 *  @param Format   Operator format
 *  @param Operator Operator name
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_SetOperator(SIM7020_t& p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator);

/** @brief              Get the current selected operator.
 *  @param p_Device     SIM7020 device object
 *  @param p_Operator   Pointer to operator
 *  @param p_Modes      Pointer to Operator mode
 *  @param p_Format     Pointer to Operator format
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_GetOperator(SIM7020_t& p_Device, SIM70XX_Operator_t* p_Operator, SIM70XX_OpMode_t* p_Mode, SIM70XX_OpForm_t* p_Format);

/** @brief              Get the available operators.
 *  @param p_Device     SIM7020 device object
 *  @param p_Operators  Pointer for list of available operators
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_GetAvailOperators(SIM7020_t& p_Device, std::vector<SIM70XX_Operator_t>* p_Operators);

/** @brief          Set the frequency band of the module.
 *  @param p_Device SIM7020 device object
 *  @param Band     Frequency band
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_SetBand(SIM7020_t& p_Device, SIM7020_Band_t Band);

/** @brief          Get the frequency band of the module.
 *  @param p_Device SIM7020 device object
 *  @param p_Band   Pointer to freuquency band
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_GetBand(SIM7020_t& p_Device, SIM7020_Band_t* p_Band);

/** @brief          Set the device functionality.
 *  @param p_Device SIM7020 device object
 *  @param Func     Device functionality
 *  @param Reset    (Optional) Reset the MT before setting the function
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_SetFunctionality(SIM7020_t& p_Device, SIM7020_Func_t Func, SIM7020_Reset_t Reset = SIM7020_RESET_NO);

/** @brief          Get the device functionality.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_GetFunctionality(SIM7020_t& p_Device);

/** @brief          Get SIM card status.
 *  @param p_Device SIM7020 device object
 *  @param p_Status Pointer so SIM card status
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_GetSIMStatus(SIM7020_t& p_Device, SIM7020_SIM_t* const p_Status);

/** @brief          Check if the SIM card is ready to use.
 *  @param p_Device SIM7020 device object
 *  @return         #true when the SIM card is ready
 */
bool SIM7020_isSIMReady(SIM7020_t& p_Device);

/** @brief          Ping the module by sending an empty 'AT'.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Ping(SIM7020_t& p_Device);

#endif /* SIM7020_H_ */