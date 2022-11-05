 /*
 * sim7080_pwrmgnt.h
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

#ifndef SIM7080_PWRMGNT_H_
#define SIM7080_PWRMGNT_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_pwrmgnt_defs.h"

/** @brief          Initialize the PSM. Must be called before enabling PSM.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PSM_Init(SIM7080_t& p_Device);

/** @brief              Enable PSM.
 *  @param p_Device     SIM7080 device object
 *  @param TAU_Base     Periodic-TAU base value
 *  @param TAU_Value    Periodic-TAU value
 *  @param Active_Base  Active-Time base value
 *  @param Active_Value Active-Time value
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PSM_Enable(SIM7080_t& p_Device, SIM7080_PSM_TAU_t TAU_Base, uint8_t TAU_Value, SIM7080_PSM_Time_t Time_Base, uint8_t Time_Value);

/** @brief          Disable PSM.
 *  @param p_Device SIM7080 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PSM_Disable(SIM7080_t& p_Device);

/** @brief          Set the PSM modem optimizations.
 *  @param p_Device SIM7080 device object
 *  @param p_Opts   Pointer to modem optimization object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PSM_SetOptimizations(SIM7080_t& p_Device, SIM7080_PSM_ModemOpts_t* p_Opts);

/** @brief          Get the PSM modem optimizations.
 *  @param p_Device SIM7080 device object
 *  @param p_Opts   Pointer to modem optimization object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PSM_GetOptimizations(SIM7080_t& p_Device, SIM7080_PSM_ModemOpts_t* p_Opts);

/** @brief          Get the status of the PSM mode.
 *  @param p_Device SIM7080 device object
 *  @return         #true when PSM mode is active.
 */
bool SIM7080_PSM_isActive(SIM7080_t& p_Device);

/** @brief          Wake up the module.
 *  @param p_Device SIM7080 device object
 *  @param Timeout  (Optional) Timeout in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PwrMgnt_WakeUp(SIM7080_t& p_Device, uint8_t Timeout = 10);

#endif /* SIM7020_PWRMGNT_H_ */