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

#include "Misc/sim7080_info.h"
#include "Definitions/sim7080_defs.h"
#include "sim70xx_tools.h"
#include "sim70xx_errors.h"

#include <stdint.h>
#include <stdbool.h>

#include "Definitions/Configs/sim7080_config_1nce.h"
#include "Definitions/Configs/sim7080_config_fusion.h"

#include <sdkconfig.h>

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param p_Config Pointer to SIM7080 device configuration object
 *  @param Timeout  Reset timeout
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, uint32_t Timeout);

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param p_Config Pointer to SIM7080 device configuration object
 *  @param Old      Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, SIM70XX_Baud_t Old);

/** @brief          Initialize the communication interface and the SIM7080 module.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param p_Config Pointer to SIM7080 device configuration object
 *  @param Timeout  (Optional) Reset timeout
 *  @param Old      (Optional) Old baudrate configuration
 *                  NOTE: Use this parameter if you want to change the baudrate of the module before the module initialization begins.
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Init(SIM7080_t* const p_Device, const SIM7080_Config_t* const p_Config, uint32_t Timeout = 10, SIM70XX_Baud_t Old = SIM_BAUD_AUTO);

/** @brief          Deinitialize the SIM7080 module.
 *  @param p_Device Pointer to SIM7080 device object
 */
void SIM7080_Deinit(SIM7080_t* const p_Device);

#ifdef CONFIG_SIM70XX_RESET_USE_HW
    /** @brief          Perform a hardware reset of the module.
     *  @param p_Device Pointer to SIM7080 device object
     */
    void SIM7080_HardReset(SIM7080_t* const p_Device);
#endif

/** @brief          Perform a software reset of the device.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param Timeout  (Optional) Timeout for the device reset in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SoftReset(const SIM7080_t* const p_Device, uint32_t Timeout = 10);

/** @brief          Set the operator for the communication.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param Mode     Operator selection mode
 *  @param Format   Operator format
 *  @param Operator Operator name
 *  @param AcT      Access technology
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetOperator(SIM7080_t* const p_Device, SIM70XX_OpMode_t Mode, SIM70XX_OpForm_t Format, std::string Operator, SIM7080_AcT_t AcT = SIM7080_ACT_USER_LTE_M1);

/** @brief          Set the frequency band of the module.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param Band     Frequency band
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetBand(SIM7080_t* const p_Device, SIM7080_Band_t Band);

/** @brief          Get the frequency band of the module.
 *  @param p_Device Pointer to SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetBand(SIM7080_t* const p_Device);

/** @brief          Set the device functionality.
 *  @param p_Device Pointer to SIM7080 device object
 *  @param Func     Device functionality
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_SetFunctionality(SIM7080_t* const p_Device, SIM7080_Func_t Func);

/** @brief          Get the device functionality.
 *  @param p_Device Pointer to SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_GetFunctionality(SIM7080_t* const p_Device);

/** @brief          Ping the module by sending an empty 'AT'.
 *  @param p_Device Pointer to SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_Ping(SIM7080_t* const p_Device);

#endif /* SIM7080_H_ */