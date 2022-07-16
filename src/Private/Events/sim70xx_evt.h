 /*
 * sim70xx_evt.h
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

#ifndef SIM70XX_EVT_H_
#define SIM70XX_EVT_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include <sdkconfig.h>

#if(defined CONFIG_SIM70XX_DEV_SIM7020)
    #include "sim7020.h"
    #include "SIM7020/sim7020_evt.h"

    #define SIM70XX_PSM_EVENT(Device, Message, isPSM)           SIM7020_Evt_HandlePSM(Device, Message, isPSM)
    #define SIM70XX_TCP_DISCONNECT_EVENT(Device, Message)       SIM7020_Evt_TCP_Disconnect(Device, Message)
    #define SIM70XX_MQTT_PUB_EVENT(Device, Message)             SIM7020_Evt_MQTT_Pub(Device, Message)
    #define SIM70XX_MQTT_DISCONNECT_EVENT(Device, Message)      SIM7020_Evt_MQTT_Disconnect(Device, Message)
    #define SIM70XX_HTTP_EVENT(Device, Message)                 SIM7020_Evt_HTTP(Device, Message)
#elif(defined CONFIG_SIM70XX_DEV_SIM7080)
    #include "sim7080.h"
    #include "SIM7080/sim7080_evt.h"

    #define SIM70XX_PSM_EVENT(Device, Message, isPSM)
    #define SIM70XX_TCP_DISCONNECT_EVENT(Device, Message)
    #define SIM70XX_MQTT_PUB_EVENT(Device, Message)
    #define SIM70XX_MQTT_DISCONNECT_EVENT(Device, Message)
    #define SIM70XX_HTTP_EVENT(Device, Message)
#endif

/** @brief          This task handels the communication with the SIM70XX module.
 *                  The task will receive a reference to a SIM70XX_TxCmd_t object and start with the transmission of the data.
 *                  The reference to the object will be destroyed after the transmission.
 *  @param p_Arg    Pointer to task parameter
 */
void SIM70XX_Evt_Task(void* p_Arg);

#endif /* SIM70XX_EVT_H_ */