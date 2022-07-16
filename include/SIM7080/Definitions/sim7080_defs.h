 /*
 * sim7080_defs.h
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

#ifndef SIM7080_DEFS_H_
#define SIM7080_DEFS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <string>
#include <vector>
#include <stdint.h>
#include <stdbool.h>
#include <algorithm>

#include <sdkconfig.h>

#include "sim70xx_defs.h"

/** @brief SIM7080 device object.
 */
typedef struct
{
    SIM70XX_UART_Conf_t UART;                               /**< UART configuration object. */
    #ifdef CONFIG_SIM70XX_RESET_USE_HW
        struct
        {
            bool Inverted;						            /**< */
            gpio_num_t Pin;						            /**< Reset pin for the module.
                                                                 NOTE: Can be set to -1 when not used. */
        } Reset_Conf;
    #endif
    struct
    {
        QueueHandle_t RxQueue;                              /**< Message receive (Module -> ESP32) queue.
                                                                 NOTE: Managed by the device driver. */
        QueueHandle_t TxQueue;                              /**< Message transmit (ESP32 -> Module) queue.
                                                                 NOTE: Managed by the device driver. */
        QueueHandle_t EventQueue;                           /**< Asynchronous event queue.
                                                                 NOTE: Managed by the device driver. */
        bool isInitialized;                                 /**< #true when the module is initialized and ready to use.
                                                                 NOTE: Managed by the device driver. */
        bool isPSM;                                         /**< #true when the module has entered PSM.
                                                                 NOTE: Managed by the device driver. */
        bool isPSMEvent;                                    /**< #true when PSM event notifications are enabled.
                                                                 NOTE: Managed by the device driver. */
        bool isActive;                                      /**< #true when the device is active and ready to use.
                                                                 NOTE: Managed by the device driver. */
        TaskHandle_t TaskHandle;                            /**< Handle of the receive task.
                                                                 NOTE: Managed by the device driver. */
    } Internal;
} SIM7080_t;

/** @brief SIM7080 device configuration object.
 */
typedef struct
{
    SIM70XX_UART_Conf_t UART;                               /**< UART configuration object. */
    #ifdef CONFIG_SIM70XX_RESET_USE_HW
        struct
        {
            bool Inverted;						            /**< */
            gpio_num_t Pin;						            /**< Reset pin for the module.
                                                                 NOTE: Can be set to -1 when not used. */
        } Reset_Conf;
    #endif
} SIM7080_Config_t;

#endif /* SIM7080_DEFS_H_ */