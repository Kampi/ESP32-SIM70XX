 /*
 * sim7080_http.h
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

#ifndef SIM7080_HTTP_H_
#define SIM7080_HTTP_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_http_defs.h"

/** @brief                  Start a new HTTP(S) post request.
 *  @param p_Device         SIM7080 device object
 *  @param Path             Request path
 *  @param ContentType      Content type
 *  @param Header           Request header
 *  @param Payload          Payload string
 *  @param p_ResponseCode   (Optional) Pointer to response code
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_HTTP_POST(SIM7080_t& p_Device, SIM7080_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, std::string Payload, uint16_t* p_ResponseCode = NULL);

/** @brief                  Start a new HTTP(S) post request.
 *  @param p_Device         SIM7080 device object
 *  @param p_Socket         Pointer to HTTP(S) socket object
 *  @param Path             Request path
 *  @param ContentType      Content type
 *  @param Header           Request header
 *  @param p_Buffer         Pointer to data buffer
 *  @param Length           Buffer length
 *  @param p_ResponseCode   (Optional) Pointer to response code
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_HTTP_POST(SIM7080_t& p_Device, SIM7080_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, const void* p_Buffer, uint32_t Length, uint16_t* p_ResponseCode = NULL);

#endif /* SIM7080_HTTP_H_ */