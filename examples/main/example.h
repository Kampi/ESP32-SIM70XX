 /*
 * example.h
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX example application.
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

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include "sim70xx.h"

#include <sdkconfig.h>

#if(CONFIG_SIMXX_DEV == 7020)
    #define DEVICE_TYPE                     SIM7020_t
#elif(CONFIG_SIMXX_DEV == 7080)
    #define DEVICE_TYPE                     SIM7080_t
#else
    #error "No device specified!"
#endif

/** @brief Start the SIM70XX examples.
 */
void StartExamples(void);

#ifdef CONFIG_DEMO_USE_SSL
    /** @brief          Configure the device for SSL.
     *  @param p_Device Pointer to device object
     */
    void SSL_Configure(DEVICE_TYPE& p_Device);
#endif

#ifdef CONFIG_DEMO_USE_NVRAM
    /** @brief          Run the NVRAM example.
     *  @param p_Device Pointer to device object
	 *	@param Key		(Optional) Key for demo application
     */
    void NVRAM_Run(DEVICE_TYPE& p_Device, std::string Key = "Demo");
#endif

#ifdef CONFIG_DEMO_USE_FS
    /** @brief          Run the file system example.
     *  @param p_Device Pointer to device object
     */
    void FileSystem_Run(DEVICE_TYPE& p_Device);
#endif

#ifdef CONFIG_DEMO_USE_TCPIP_CLIENT
    /** @brief              Run the TCP client example with a form-data HTTP request.
     *  @param p_Device     Pointer to device object
     *  @param p_Opts       (Optional) Pointer to additional options
     */
    void TCP_Client_Run_FormData(DEVICE_TYPE& p_Device, void* p_Opts = NULL);

    /** @brief          Run the TCP client example with a JSON HTTP request.
     *  @param p_Device Pointer to device object
     *  @param p_Opts   (Optional) Pointer to additional options
     */
    void TCP_Client_Run_JSON(DEVICE_TYPE& p_Device, void* p_Opts = NULL);
#endif

#ifdef CONFIG_DEMO_USE_UDP_CLIENT
    /** @brief          Run the UDP example.
     *  @param p_Device Pointer to device object
     */
    void UDP_Client_Run(DEVICE_TYPE& p_Device);
#endif

#ifdef CONFIG_DEMO_USE_EMAIL
    /** @brief          Run the E-Mail example.
     *  @param p_Device Pointer to device object
     *  @param p_Opts   (Optional) Pointer to additional options
     */
    void EMail_Run(DEVICE_TYPE& p_Device, void* p_Opts = NULL);
#endif

#ifdef CONFIG_DEMO_USE_SNTP
    /** @brief          Run the NTP example.
     *  @param p_Device Pointer to device object
     */
    void NTP_Run(DEVICE_TYPE& p_Device);
#endif

#ifdef CONFIG_DEMO_USE_MQTT
    /** @brief          Run the MQTT example.
     *  @param p_Device Pointer to device object
     *  @param p_Opts   (Optional) Pointer to additional options
     *  @param SubTopic (Optional) Subscription topic
     *  @param PubTopic (Optional) Publish topic
     */
    void MQTT_Run(DEVICE_TYPE& p_Device, void* p_Opts = NULL, std::string SubTopic = "foo/bar", std::string PubTopic = "foo/bar");
#endif

#ifdef CONFIG_DEMO_USE_COAP
    /** @brief          Run the CoAP example.
     *  @param p_Device Pointer to device object
     */
    void CoAP_Run(DEVICE_TYPE& p_Device);
#endif

#ifdef CONFIG_DEMO_USE_HTTP
    /** @brief          Run the HTTP(S) example.
     *  @param p_Device Pointer to device object
     */
    void HTTP_Run(DEVICE_TYPE& p_Device);
#endif

#endif /* EXAMPLE_H_ */