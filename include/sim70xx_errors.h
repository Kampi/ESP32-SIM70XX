 /*
 * sim70xx_errors.h
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

#ifndef SIM70XX_ERRORS_H_
#define SIM70XX_ERRORS_H_

#include <stdint.h>

#include <sdkconfig.h>

#ifndef CONFIG_SIM70XX_MISC_ERROR_BASE
    #define CONFIG_SIM70XX_MISC_ERROR_BASE                      0x9000
#endif

typedef int SIM70XX_Error_t;

/** @brief SIM70XX error base.
 */
#define SIM70XX_ERR_BASE                                        CONFIG_SIM70XX_MISC_ERROR_BASE

/** @brief The device has not reported a error.
 */
#define SIM70XX_ERR_OK                                          SIM70XX_ERR_BASE + 0

/** @brief Invalid function argument.
 */
#define SIM70XX_ERR_INVALID_ARG                                 SIM70XX_ERR_BASE + 1

/** @brief The module or the module interface is not initialized. Please call the module initialization function "SIM70xy_Init" first!
 */
#define SIM70XX_ERR_NOT_INITIALIZED                             SIM70XX_ERR_BASE + 2

/** @brief The Module or the module interface connection is not ready.
 */
#define SIM70XX_ERR_NOT_READY                                   SIM70XX_ERR_BASE + 3

/** @brief The module is in the wrong device state.
 */
#define SIM70XX_ERR_INVALID_STATE                               SIM70XX_ERR_BASE + 4

/** @brief When the status response from the module is not "OK" or when the module is busy.
 */
#define SIM70XX_ERR_FAIL                                        SIM70XX_ERR_BASE + 5

/** @brief When the module reports a timeout.
 */
#define SIM70XX_ERR_TIMEOUT                                     SIM70XX_ERR_BASE + 6

/** @brief When the receive task can not get started or when the communication queues can not be created.
 */
#define SIM70XX_ERR_NO_MEM                                      SIM70XX_ERR_BASE + 7

/** @brief Common message queue error.
 */
#define SIM70XX_ERR_QUEUE_ERR                                   SIM70XX_ERR_BASE + 8

/** @brief The message queue is full and no more items can be places on the queue.
 */
#define SIM70XX_ERR_QUEUE_FULL                                  SIM70XX_ERR_BASE + 9

/** @brief The message queue is empty.
 */
#define SIM70XX_ERR_QUEUE_EMPTY                                 SIM70XX_ERR_BASE + 10

/** @brief Socket not created.
 */
#define SIM70XX_ERR_NOT_CREATED                                 SIM70XX_ERR_BASE + 11

/** @brief Socket not connected.
 */
#define SIM70XX_ERR_NOT_CONNECTED                               SIM70XX_ERR_BASE + 11

/** @brief      Generic error check macro.
 *  @param Func Function call
 */
#define SIM70XX_ERROR_CHECK(Func)                               do                                                                                      \
                                                                {                                                                                       \
                                                                    SIM70XX_Error_t Error = Func;                                                       \
                                                                    if(Error != SIM70XX_ERR_OK)                                                         \
                                                                    {                                                                                   \
																		ESP_LOGE("SIM70XX_Error", "Error: 0x%X", Error);                                \
                                                                        ESP_LOGE("SIM70XX_Error", "  %s:%d (%s)", __FILE__, __LINE__, __FUNCTION__);    \
                                                                        return Error;                                                                   \
                                                                    }                                                                                   \
                                                                } while(0);

#endif /* SIM70XX_ERRORS_H_ */