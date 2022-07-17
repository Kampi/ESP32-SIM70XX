 /*
 * sim7020_http.h
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

#ifndef SIM7020_HTTP_H_
#define SIM7020_HTTP_H_

#include "sim7020_defs.h"
#include "sim70xx_errors.h"
#include "sim7020_http_defs.h"

/** @brief          Create a HTTP(S) socket.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param Host     Host address
 *  @param p_Socket Pointer to HTTP(S) socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t* const p_Device, std::string Host, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Create a HTTP(S) socket.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Socket Pointer to HTTP(S) socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Open a URL and create a HTTP(S) socket.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Socket Pointer to HTTP(S) socket object
 *  @param Timeout  (Optional) Socket timeout
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Connect(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket, uint16_t Timeout = 60);

/** @brief                  Start a new HTTP(S) post request.
 *  @param p_Device         Pointer to SIM7020 device object
 *  @param p_Socket         Pointer to HTTP(S) socket object
 *  @param Path             Request path
 *  @param ContentType      Content type
 *  @param Header           Request header
 *  @param Payload          Payload string
 *  @param p_ResponseCode   (Optional) Pointer to response code
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, std::string Payload, uint16_t* p_ResponseCode = NULL);

/** @brief                  Start a new HTTP(S) post request.
 *  @param p_Device         Pointer to SIM7020 device object
 *  @param p_Socket         Pointer to HTTP(S) socket object
 *  @param Path             Request path
 *  @param ContentType      Content type
 *  @param Header           Request header
 *  @param p_Buffer         Pointer to data buffer
 *  @param Length           Buffer length
 *  @param p_ResponseCode   (Optional) Pointer to response code
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, const void* p_Buffer, uint32_t Length, uint16_t* p_ResponseCode = NULL);

/** @brief                  
 *  @param p_Device         Pointer to SIM7020 device object
 *  @param p_Socket         Pointer to HTTP(S) socket object
 *  @param p_Buffer         Pointer to data buffer
 *                          NOTE: The memory for the buffer is dynamic memory and must be freed after usage!
 *  @param p_Length         Payload length
 *  @param p_ResponseCode   (Optional) Pointer to response code
 *  @return                 SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_GET(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, uint8_t** p_Buffer, uint32_t* p_Length, uint16_t* p_ResponseCode = NULL);

/** @brief          Disconnect a HTTP(S) socket.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Socket Pointer to HTTP(S) socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Disconnect(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Destroy a HTTP(S) socket.
 *  @param p_Device Pointer to SIM7020 device object
 *  @param p_Socket Pointer to HTTP(S) socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Destroy(SIM7020_t* const p_Device, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Destroy all active HTTP(S) sockets.
 *  @param p_Device Pointer to SIM7020 device object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_DestroyAllSockets(SIM7020_t* const p_Device);

/** @brief          Add a key / value pair to the message header.
 *  @param Key      Header key
 *  @param Value    Header value
 *  @param p_Header Pointer to header string
 */
void SIM7020_HTTP_AddToHeader(std::string Key, std::string Value, std::string* p_Header);

#endif /* SIM7020_HTTP_H_ */