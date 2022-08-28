 /*
 * sim7020_coap.h
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

#ifndef SIM7020_COAP_H_
#define SIM7020_COAP_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_coap_defs.h"

/** @brief          Create a CoAP socket.
 *  @param p_Device SIM7020 device object
 *  @param Server   CoAP server address
 *  @param Port     CoAP broker port
 *  @param p_Socket Pointer to CoAP socket object
 *  @param CID      (Optional) Context Identifier
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, std::string Server, uint16_t Port, SIM7020_CoAP_Socket_t* p_Socket, uint8_t CID = 1);

/** @brief          Create a CoAP socket.
 *  @param p_Device SIM7020 device object
 *  @param Host     Host address
 *  @param p_Socket Pointer to CoAP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_CoAP_Create(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket);

/** @brief          Transmit a CoAP message.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to CoAP socket object
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   Data length (maximum 512 bytes)
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_CoAP_Transmit(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket, const void* p_Buffer, uint16_t Length);

/** @brief          Close a CoAP connection and release the socket.
 *  @param p_Device SIM7020 device object
 *  @param Host     Host address
 *  @param p_Socket Pointer to CoAP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_CoAP_Destroy(SIM7020_t& p_Device, SIM7020_CoAP_Socket_t* p_Socket);

#endif /* SIM7020_COAP_H_ */