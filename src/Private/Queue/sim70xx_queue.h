 /*
 * sim70xx_queue.h
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

#ifndef SIM70XX_QUEUE_H_
#define SIM70XX_QUEUE_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <sdkconfig.h>

#include "sim70xx_errors.h"

#ifndef CONFIG_SIM70XX_QUEUE_LENGTH
    #define CONFIG_SIM70XX_QUEUE_LENGTH                         16
#endif

/** @brief  Push a item to the message queue.
 *          The item will be deleted and an error is generated when the message queue is full.
 */
#define SIM70XX_PUSH_QUEUE(Queue, Command)                      do                                                              \
                                                                {                                                               \
                                                                    if(xQueueSend(Queue, &Command, portMAX_DELAY) != pdPASS)    \
                                                                    {                                                           \
                                                                        delete Command;                                         \
                                                                        return SIM70XX_ERR_QUEUE_FULL;                          \
                                                                    }                                                           \
                                                                } while(0);


/** @brief SIM70XX Tx command (ESP32 -> Module) object definition.
 */
typedef struct
{
    std::string Command;                            /**< AT command string. */
    bool recData;                                   /**< Set to #true to receive data from the command. */
    uint32_t Timeout;                               /**< Response timeout in seconds. */
    uint8_t Lines;                                  /**< Number of response lines.
                                                         NOTE: Set to 0 to receive until a empty line is received. */
} SIM70XX_TxCmd_t;

/** @brief  Command response object for the communication task
 *          NOTE: The object is completly handled by the driver.
 */
typedef struct
{
    std::string Response;                           /**< Command response (if a response was requested). */
    std::string Status;                             /**< Command status. */
    bool isError;                                   /**< Set to #true when the module has reported an error. */
    bool isTimeout;                                 /**< Set to #true when the module has not send an answer before a timeout. */
    bool recData;                                   /**< Set to #true to receive data from the command. */
    uint8_t Lines;                                  /**< Number of response lines to receive (if a response was requested). */
    uint32_t Length;                                /**< Length of the command in bytes. */
    uint32_t Timeout;                               /**< Response timeout in seconds. */
} SIM70XX_CmdResp_t;

/** @brief              Process the receive queue and get the next item from the queue.
 *                      NOTE: This function is used to get the immediate response and the status message from the device after transmitting a command.
 *                            This function must be called after transmitting a command to clear the receive queue.
 *                            This function will handle the allocated memory from the receive task.
 *  @param Queue        Message queue
 *  @param p_Response   (Optional) Pointer to command response string
 *  @param p_Status     (Optional) Pointer to response status string
 *  @return             SIM70XX_ERR_OK when successful
 *                      SIM70XX_ERR_INVALID_ARG when an invalid argument is passed into the function
 *                      SIM70XX_ERR_INVALID_STATE when the device is not initialized
 *                      SIM70XX_ERR_QUEUE_EMPTY when the receive queue is empty
 *                      SIM70XX_ERR_INVALID_RESPONSE when the item can not be popped from the queue
 *                      SIM70XX_ERR_FAIL when the device has reported an error
 *                      SIM70XX_ERR_TIMEOUT when a communication timeout has occured
 */
SIM70XX_Error_t SIM70XX_Queue_PopItem(QueueHandle_t Queue, std::string* p_Response = NULL, std::string* p_Status = NULL);

/** @brief          Get the items in the recieve queue.
 *  @param Queue    Message queue
 *  @return         Item count in the receive queue
 */
uint32_t SIM70XX_Queue_GetItems(QueueHandle_t Queue);

/** @brief          Wait for items to receive.
 *  @param Queue    Message queue
 *  @param p_Active Pointer to active state of the device
 *  @param Timeout  (Optional) Wait timeout in seconds
 *  @param Items    (Optional) Number of items to wait for
 *  @return         #true when the items are placed in the queue
 */
bool SIM70XX_Queue_Wait(QueueHandle_t Queue, bool* p_Active, uint32_t Timeout = 1, uint32_t Items = 1);

/** @brief          Reset the recieve queue and discard all items.
 *  @param Queue    Message queue
 */
void SIM70XX_Queue_Reset(QueueHandle_t Queue);

/** @brief          Wait and receive the data after for an asynchronous message string.
 *                  NOTE: This function can be used when the module will send the response some time after the status code.
 *                        This function will handle the memory allocation from the asynchronous event loop.
 *  @param Queue    Event message queue
 *  @param Filter   Filter string to wait for
 *  @param p_Event  Pointer to event message
 *  @return         #true when a event was received
 */
bool SIM70XX_Queue_isEvent(QueueHandle_t Queue, std::string Filter, std::string* p_Event);

#endif /* SIM70XX_QUEUE_H_ */