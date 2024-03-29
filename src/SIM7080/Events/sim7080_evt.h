 /*
 * sim7080_evt.h
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

#ifndef SIM7080_EVT_H_
#define SIM7080_EVT_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include "sim7080.h"

#include <sdkconfig.h>

#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
    /** @brief              TCP/IP disconnect event handler.
     *                      This function will filter out the disconnect message from the event message.
     *  @param p_Device     Pointer to device
     *  @param p_Message    Pointer to message string
     */
    void SIM7080_Evt_on_TCP_Disconnect(SIM7080_t* const p_Device, std::string* p_Message);

    /** @brief              TCP/IP data ready event handler.
     *                      This function will filter out the receive message from the event message.
     *  @param p_Device     Pointer to device
     *  @param p_Message    Pointer to message string
     */
    void SIM7080_Evt_on_TCP_DataReady(SIM7080_t* const p_Device, std::string* p_Message);
#endif

#endif /* SIM7080_EVT_H_ */