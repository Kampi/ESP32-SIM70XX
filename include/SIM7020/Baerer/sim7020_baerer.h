 /*
 * sim7020_baerer.h
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

#ifndef SIM7020_BAERER_H_
#define SIM7020_BAERER_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_baerer_defs.h"

/** @brief          Deattach the module to GPRS service.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_GRPS_Attach(SIM7020_t& p_Device);

/** @brief          Deattach the module from GPRS service.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_GRPS_Deattach(SIM7020_t& p_Device);

/** @brief                  Check if the module is connected with the GPRS service.
 *  @param p_Device         SIM7020 device object
 *  @param p_Deactivated    (Optional) Pointer to context activation status.
 *                          Set to #false when the context isn´t activated
 *  @return                 #true when the module is connected to the GPRS service
 */
bool SIM7020_Baerer_GRPS_isAttached(SIM7020_t& p_Device, bool* const p_Deactivated = NULL);

/** @brief          Define a PDP context.
 *  @param p_Device SIM7020 device object
 *  @param APN      Access Point Name (APN) configuration
 *  @param p_PDP    Pointer to PDP context
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_PDP_Configure(SIM7020_t& p_Device, SIM70XX_APN_t APN, SIM7020_PDP_Context_t* const p_PDP);

/** @brief          Enable the given PDP context.
 *  @param p_Device SIM7020 device object
 *  @param p_PDP    Pointer to PDP context
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_PDP_Enable(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP);

/** @brief          Disable the given PDP context.
 *  @param p_Device SIM7020 device object
 *  @param p_PDP    Pointer to PDP context
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_PDP_Disable(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_PDP);

/** @brief          Disable all PDP contextes.
 *  @param p_Device SIM7020 device object
 *  @param p_PDP    Pointer to PDP context
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_Baerer_PDP_DisableAll(SIM7020_t& p_Device);

/** @brief              Get the status of all PDP contextes.
 *  @param p_Device     SIM7020 device object
 *  @param p_Context    Pointer to SIM7020 PDP context object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_GetStatus(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_Context);

/** @brief              Read the dynamic PDP (Packet Data Protocol) context parameters.
 *  @param p_Device     SIM7020 device object
 *  @param p_Context    Pointer to SIM7020 PDP context object
 *  @param p_Params     Pointer to PDP context dynamic parameters
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_ReadDynamicParameters(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_Context, SIM7020_PDP_Params_t* const p_Params);

/** @brief          Set and enable the APN.
 *  @param p_Device SIM7020 device object
 *  @param APN      APN configuration object
 *  @param p_PDP    Pointer to PDP context
 *  @param Timeout  (Optional) Wait for connection timeout in seconds
 *  @return         SIM70XX_ERR_OK when successful
 *                  SIM70XX_ERR_PDP_NOT_ACTIVE when the PDP context is not activated
 */
SIM70XX_Error_t SIM7020_Baerer_SetAPN(SIM7020_t& p_Device, SIM70XX_APN_t APN, SIM7020_PDP_Context_t* const p_PDP, uint32_t Timeout = 60);

#endif /* SIM7020_BAERER_H_ */