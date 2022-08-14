 /*
 * sim7080_dns.h
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

#ifndef SIM7080_DNS_H_
#define SIM7080_DNS_H_

#include "sim7080_defs.h"
#include "sim70xx_errors.h"
#include "sim7080_dns_defs.h"

/** @brief          Parse a given host name to get the IP address.
 *  @param p_Device SIM7080 device object
 *  @param Host     Host name
 *  @param p_IP     Pointer to resolved IP address
 *  @param p_Error  (Optional) DNS error code
 *  @param Timeout  (Optional) Message timeout for each the request in seconds
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM7080_DNS_FetchAddress(SIM7080_t& p_Device, std::string Host, std::string* p_IP, SIM7080_DNS_Error_t* p_Error = NULL, uint32_t Timeout = 60);

#endif /* SIM7080_DNS_H_ */