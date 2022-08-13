 /*
 * sim7080_pdp.h
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

#ifndef SIM7080_PDP_H_
#define SIM7080_PDP_H_

#include "sim70xx_errors.h"
#include "sim7080_defs.h"
#include "sim7080_pdp_defs.h"

/** @brief          Define a GPRS PDP context.
 *  @param p_Device SIM7080 device object
 *  @param Type     Packet Data Protocol (PDP) type
 *  @param APN      Access Point Name (APN) configuration
 *  @param PDP      (Optional) PDP context ID
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PDP_GPRS_Define(SIM7080_t& p_Device, SIM7080_PDP_GPRS_Type_t Type, SIM70XX_APN_t APN, uint8_t PDP = 1);

/** @brief          Check if the module is connected with the GPRS service.
 *  @param p_Device SIM7080 device object
 *  @return         #true when the module is connected to the GPRS service
 */
bool SIM7080_PGP_GRPS_isAttached(SIM7080_t& p_Device);

/** @brief          Define a IP PDP context.
 *  @param p_Device SIM7080 device object
 *  @param Type     Packet Data Protocol (PDP) type
 *  @param APN      Access Point Name (APN) configuration
 *  @param PDP      (Optional) PDP context ID
 *  @param Auth     (Optional) Authentication method for APN
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PDP_IP_Configure(SIM7080_t& p_Device, SIM7080_PDP_IP_Type_t Type, SIM70XX_APN_t APN, uint8_t PDP = 0, SIM7080_PDP_IP_Auth_t Auth = SIM7080_PDP_IP_AUTH_NONE);

/** @brief          Execute a IP PDP action for a specific PDP context.
 *  @param p_Device SIM7080 device object
 *  @param PDP      PDP context ID
 *  @param Action   PDP action
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PDP_IP_Action(SIM7080_t& p_Device, uint8_t PDP, SIM7080_PDP_Action_t Action);

/** @brief              Get the network status.
 *  @param p_Device     SIM7080 device object
 *  @param p_Networks   Pointer to network status list
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_PDP_IP_CheckNetworks(SIM7080_t& p_Device, std::vector<SIM7080_PDP_Network_t>* p_Networks);

#endif /* SIM7080_PDP_H_ */