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

/** @brief          Get the number response messages from the response queue.
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         Number of items in the response queue.
 */
inline __attribute__((always_inline)) uint32_t SIM7020_HTTP_GetResponseItems(SIM7020_HTTP_Socket_t* p_Socket)
{
    if((p_Socket == NULL) || (p_Socket->Internal.ResponseQueue == NULL))
    {
        return 0;
    }

    return uxQueueMessagesWaiting(p_Socket->Internal.ResponseQueue);
}

/** @brief          Create a HTTP socket.
 *  @param p_Device SIM7020 device object
 *  @param Host     Host address
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t& p_Device, std::string Host, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Create a HTTP socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Create(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Open a URL and create a HTTP socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to HTTP socket object
 *  @param Timeout  (Optional) Socket timeout
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Connect(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, uint16_t Timeout = 60);

/** @brief              Start a new HTTP POST request.
 *  @param p_Device     SIM7020 device object
 *  @param p_Socket     Pointer to HTTP socket object
 *  @param Path         Request path
 *  @param ContentType  Content type
 *  @param Header       Request header
 *  @param Payload      Payload string
 *  @param p_Response   (Optional) Pointer to HTTP response object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, std::string Payload, SIM7020_HTTP_Response_t* p_Response = NULL);

/** @brief              Start a new HTTP POST request.
 *  @param p_Device     SIM7020 device object
 *  @param p_Socket     Pointer to HTTP socket object
 *  @param Path         Request path
 *  @param ContentType  Content type
 *  @param Header       Request header
 *  @param p_Buffer     Pointer to data buffer
 *  @param Length       Buffer length
 *  @param p_Response   (Optional) Pointer to HTTP response object
 *  @return             SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_POST(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path, std::string ContentType, std::string Header, const void* p_Buffer, uint32_t Length, SIM7020_HTTP_Response_t* p_Response = NULL);

/** @brief          Start a new HTTP GET request.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_GET(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, std::string Path);

/** @brief              Get the next HTTP response from the response queue.
 *  @param p_Device     SIM7020 device object
 *  @param p_Socket     Pointer to HTTP socket object
 *  @param p_Response   Pointer to HTTP response object
 *  @return             SIM70XX_ERR_OK when successful
 *                      SIM70XX_ERR_QUEUE_EMPTY when no item is available
 */
SIM70XX_Error_t SIM7020_HTTP_GetResponse(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket, SIM7020_HTTP_Response_t* p_Response);

/** @brief          Disconnect a HTTP socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Disconnect(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket);

/** @brief          Destroy a HTTP socket.
 *  @param p_Device SIM7020 device object
 *  @param p_Socket Pointer to HTTP socket object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7020_HTTP_Destroy(SIM7020_t& p_Device, SIM7020_HTTP_Socket_t* p_Socket);

#endif /* SIM7020_HTTP_H_ */