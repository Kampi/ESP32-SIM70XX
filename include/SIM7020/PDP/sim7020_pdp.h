 /*
 * sim7020_pdp.h
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

#ifndef SIM7020_PDP_H_
#define SIM7020_PDP_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_pdp_defs.h"

/** @brief          Check if the module is connected with the GPRS service
 *  @param p_Device SIM7020 device object
 *  @return         #true when the module is connected to the GPRS service
 */
bool SIM7020_PGP_GPRS_isAttached(SIM7020_t& p_Device);

/** @brief              Get the PDP contexte.
 *  @param p_Device     SIM7020 device object
 *  @param p_Context    Pointer to SIM7020 PDP context object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_GetContext(SIM7020_t& p_Device, SIM7020_PDP_Context_t* const p_Context);

/** @brief          PDP Context activate or deactivate.
 *  @param p_Device SIM7020 device object
 *  @param Enable   Enable / Disable
 *  @param Context  (Optional) PDP context ID
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_Switch(SIM7020_t& p_Device, bool Enable, uint8_t Context = 1);

/** @brief          Get the status of all PDP contextes.
 *  @param p_Device SIM7020 device object
 *  @param p_Status Pointer to PDP Context status list
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_GetStatus(SIM7020_t& p_Device, std::vector<SIM7020_PDP_Status_t>* p_Status);

/** @brief          Read the dynamic PDP (Packet Data Protocol) context parameters.
 *  @param p_Device SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_PDP_ReadDynamicParameters(SIM7020_t& p_Device);

#endif /* SIM7020_PDP_H_ */