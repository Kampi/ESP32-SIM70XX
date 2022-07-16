 /*
 * sim70xx_defs.h
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

#ifndef SIM70XX_DEFS_H_
#define SIM70XX_DEFS_H_

#include <driver/gpio.h>
#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <stdint.h>
#include <stdbool.h>

#include <sdkconfig.h>

/** @brief Supported baudrates by the SIM70XX devices.
 */
typedef enum
{
    SIM_BAUD_AUTO       = 0,                        /**< Automatic baudrate selection. */
    SIM_BAUD_4800       = 4800,                     /**< Baud rate 4800. */
    SIM_BAUD_9600       = 9600,                     /**< Baud rate 9600. */
    SIM_BAUD_115200     = 115200,                   /**< Baud rate 115200. */
    SIM_BAUD_230400     = 230400,                   /**< Baud rate 230400. */
    SIM_BAUD_460800     = 460800,                   /**< Baud rate 460800. */
    SIM_BAUD_921600     = 921600,                   /**< Baud rate 921600. */
} SIM70XX_Baud_t;

/** @brief SIM70XX UART configuration object definition.
 */
typedef struct
{
    gpio_num_t Rx;                                  /**< Rx pin number. */
    gpio_num_t Tx;                                  /**< Tx pin number. */
    uart_port_t Interface;                          /**< Serial interface used by the device SIM70XX driver. */
    SemaphoreHandle_t Lock;                         /**< */
    SIM70XX_Baud_t Baudrate;                        /**< Baud rate for the serial communication interface.
                                                         NOTE: Managed by the device driver. */
    bool isInitialized;                             /**< #true when the interface is initialized.
                                                         NOTE: Managed by the device driver. */
} SIM70XX_UART_Conf_t;

#endif /* SIM70XX_DEFS_H_ */