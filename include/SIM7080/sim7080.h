 /*
 * sim7080.h
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

#ifndef SIM7080_H_
#define SIM7080_H_

#include <stdint.h>
#include <stdbool.h>

#include "sim7080_pdp.h"
#include "sim7080_info.h"
#include "sim7080_defs.h"
#include "sim70xx_tools.h"
#include "sim70xx_errors.h"

#include "sim7080_config_1nce.h"
#include "sim7080_config_fusion.h"

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_DRIVER_WITH_FS
    #include "sim7080_fs.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_DNS
    #include "sim7080_dns.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
    #include "sim7080_tcpip.h"
    #include "sim7080_config_ping.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_NTP
    #include "sim7080_ntp.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_EMAIL
    #include "sim7080_email.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_SSL
    #include "sim7080_ssl.h"
#endif

/** @brief          Check if the module is initialized.
 *  @param p_Device SIM7080 device object
 *  @return         #true when the module is initialized
 */
inline __attribute__((always_inline)) bool SIM7080_isInitialized(SIM7080_t& p_Device)
{
    return p_Device.Internal.isInitialized;
}

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 device configuration object
 *  @param Timeout  Reset timeout
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, uint32_t Timeout);

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 device configuration object
 *  @param Old      Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, SIM70XX_Baud_t Old);

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device SIM7080 device object
 *  @param p_Config SIM7080 device configuration object
 *  @param Timeout  (Optional) Reset timeout
 *  @param Old      (Optional) Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t& p_Device, const SIM7080_Config_t& p_Config, uint32_t Timeout = 10, SIM70XX_Baud_t Old = SIM_BAUD_AUTO);

/** @brief          Deinitialize the SIM7080 module.
 *  @param p_Device SIM7080 device object
 */
void SIM7080_Deinit(SIM7080_t& p_Device);

/** @brief          Perform a software reset of the device.
 *  @param p_Device SIM7080 device object
 *  @param Timeout  (Optional) Timeout for the device reset in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SoftReset(SIM7080_t& p_Device, uint32_t Timeout = 60);

/** @brief          Automatically set an enable an IP APN.
 *  @param p_Device SIM7080 device object
 *  @param APN      APN configuration object
 *  @param PDP      (Optional) PDP context ID
 *  @param Timeout  (Optional) Wait for connection timeout in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_IP_AutoAPN(SIM7080_t& p_Device, SIM70XX_APN_t APN, uint8_t PDP = 0, uint32_t Timeout = 60);

/** @brief          Manually set an enable an IP APN.
 *  @param p_Device SIM7080 device object
 *  @param APN      APN configuration object
 *  @param PDP      (Optional) PDP context ID
 *  @param Timeout  (Optional) Wait for connection timeout in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_IP_ManualAPN(SIM7080_t& p_Device, SIM70XX_APN_t APN, uint8_t PDP = 0, uint32_t Timeout = 10);

/** @brief          Set the operator for the communication.
 *  @param p_Device SIM7080 device object
 *  @param Mode     Operator selection mode
 *  @param Format   Operator format
 *  @param Operator Operator name
 *  @param AcT      (Optional) Access technology
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetOperator(SIM7080_t& p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator, SIM7080_AcT_t AcT = SIM7080_ACT_USER_LTE_M1);

/** @brief          
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetCurrentOperator(SIM7080_t& p_Device);

/** @brief          Configure the frequency bands for CAT-M or NB-IoT mode.
 *  @param p_Device SIM7080 device object
 *  @param Mode     Selected mode
 *  @param Bands    List with frequency bands
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetBandConfig(SIM7080_t& p_Device, SIM7080_Mode_t Mode, std::vector<uint8_t> Bands);

/** @brief          Get the frequency bands for CAT-M or NB-IoT mode.
 *  @param p_Device SIM7080 device object
 *  @param Mode     Selected mode
 *  @param p_Band   Pointer to frequency band list
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetBandConfig(SIM7080_t& p_Device, SIM7080_Mode_t Mode, std::vector<uint8_t>* p_Bands);

/** @brief          Set the frequency band of the module.
 *  @param p_Device SIM7080 device object
 *  @param Band     Frequency band
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetBand(SIM7080_t& p_Device, SIM7080_Band_t Band);

/** @brief          Get the frequency band of the module.
 *  @param p_Device SIM7080 device object
 *  @param p_Band   Pointer to freuquency band
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetBand(SIM7080_t& p_Device, SIM7080_Band_t* p_Band);

/** @brief          Set the preferred network mode.
 *  @param p_Device SIM7080 device object
 *  @param Mode     Preferred mode selection
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetNetMode(SIM7080_t& p_Device, SIM7080_NetMode_t Mode);

/** @brief          Get the preferred network mode.
 *  @param p_Device SIM7080 device object
 *  @param p_Mode   Pointer to preferred mode selection
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetNetMode(SIM7080_t& p_Device, SIM7080_NetMode_t* p_Mode);

/** @brief          Set the preffered selection between CAT-M and NB-IoT.
 *  @param p_Device SIM7080 device object
 *  @param Mode     Preferred mode selection
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetSelectedMode(SIM7080_t& p_Device, SIM7080_Mode_t Mode);

/** @brief          Get the preffered selection between CAT-M and NB-IoT.
 *  @param p_Device SIM7080 device object
 *  @param p_Mode   Pointer to referred selection
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetSelection(SIM7080_t& p_Device, SIM7080_Mode_t* p_Mode);

/** @brief          Set the device functionality.
 *  @param p_Device SIM7080 device object
 *  @param Func     Device functionality
 *  @param Reset    (Optional) Reset the MT before setting the function
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetFunctionality(SIM7080_t& p_Device, SIM7080_Func_t Func, SIM7080_Reset_t Reset = SIM7080_RESET_NO);

/** @brief          Get the device functionality.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetFunctionality(SIM7080_t& p_Device);

/** @brief          Get SIM card status.
 *  @param p_Device SIM7080 device object
 *  @param p_Status Pointer so SIM card status
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetSIMStatus(SIM7080_t& p_Device, SIM7080_SIM_t* const p_Status);

/** @brief          Check if the SIM card is ready to use.
 *  @param p_Device SIM7080 device object
 *  @return         #true when the SIM card is ready
 */
bool SIM7080_isSIMReady(SIM7080_t& p_Device);

/** @brief          Ping the module by sending an empty 'AT'.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Ping(SIM7080_t& p_Device);

#endif /* SIM7080_H_ */