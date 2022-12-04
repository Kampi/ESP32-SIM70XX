 /*
 * sim7080_fota_defs.h
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

#ifndef SIM7080_FOTA_DEFS__H_
#define SIM7080_FOTA_DEFS__H_

/** @brief SIM7080 HTTP FOTA status code definitions.
 */
typedef enum
{
    SIM7080_FOTA_HTTP_CONT          = 100,          /**< Continue. */
    SIM7080_FOTA_HTTP_OK            = 200,          /**< Everything okay. */
    SIM7080_FOTA_HTTP_PART          = 206,          /**< Partial Content. */
    SIM7080_FOTA_HTTP_BAD_REQ       = 400,          /**< Bad request. */
    SIM7080_FOTA_HTTP_NOT_FOUND     = 404,          /**< Destination not found. */
    SIM7080_FOTA_HTTP_REQ_TIMEOUT   = 408,          /**< Request timeout. */
    SIM7080_FOTA_HTTP_SERVER_ERROR  = 500,          /**< Internal server error. */
    SIM7080_FOTA_HTTP_PDU           = 600,          /**< Not HTTP PDU. */
    SIM7080_FOTA_HTTP_NET_ERROR     = 601,          /**< Network error. */
    SIM7080_FOTA_HTTP_NO_MEM        = 602,          /**< No memory. */
    SIM7080_FOTA_HTTP_DNS_ERROR     = 603,          /**< DNS error. */
    SIM7080_FOTA_HTTP_BUSY          = 604,          /**< Stack busy. */
    SIM7080_FOTA_HTTP_SSL           = 620,          /**< SSL continue. */
} SIM7080_FOTA_HTTP_Status_t;

#endif /* SIM7080_FOTA_DEFS__H_ */