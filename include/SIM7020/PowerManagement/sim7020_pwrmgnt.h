 /*
 * sim7020_pwrmgnt.h
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

#ifndef SIM7020_PWRMGNT_H_
#define SIM7020_PWRMGNT_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "PowerManagement/sim7020_pwrmgnt_defs.h"

/** @brief              Enable the power saving mode (PSM).
 *  @param p_Device     Pointer to SIM7020 device object
 *  @param Base_T       TAU base (T3412)
 *  @param Value_T      TAU value (T3412)
 *  @param Base_A       Active Time base (T3324)
 *  @param Value_A      Active Time value (T3324)
 *  @param UseRetention (Optional) Use socket scene retention
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PSM_Enable(const SIM7020_t* const p_Device, uint8_t Base_T, uint8_t Value_T, uint8_t Base_A, uint8_t Value_A, bool UseRetention = false);

/** @brief          Disable the power saving mode (PSM).
 *  @param p_Device Pointer to SIM7020 device object
 *  @param Mode     PSM mode
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PSM_Disable(const SIM7020_t* const p_Device, SIM7020_PSM_Enable_t Mode);

/** @brief          Get the current power saving mode (PSM).
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Mode   Pointer to PSM mode
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PSM_GetMode(const SIM7020_t* const p_Device, SIM7020_PSM_Enable_t* p_Mode);

/** @brief          Get the status of the PSM event notifications.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Enable Event notification status
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PSM_GetEventStatus(const SIM7020_t* const p_Device, bool* p_Enable);

/** @brief          Set the status of the PSM event notifications.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param Enable   Enable / Disable event notifications
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PSM_SetEventStatus(const SIM7020_t* const p_Device, bool Enable);

#endif /* SIM7020_PWRMGNT_H_ */