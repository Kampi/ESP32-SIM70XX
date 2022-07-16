 /*
 * sim70xx_queue.cpp
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

#include <esp_log.h>

#include "sim70xx_queue.h"
#include "sim70xx_tools.h"

static const char* TAG = "SIM70XX_Queue";

SIM70XX_Error_t SIM70XX_Queue_PopItem(QueueHandle_t Queue, std::string* p_Response, std::string* p_Status)
{
    uint32_t MessagesInQueue;
    SIM70XX_CmdResp_t* Rx;
    SIM70XX_Error_t Error = SIM70XX_ERR_OK;

    if(Queue == NULL)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    // Check if the queue is empty.
    MessagesInQueue = uxQueueMessagesWaiting(Queue);
    if(MessagesInQueue == 0)
    {
        return SIM70XX_ERR_QUEUE_EMPTY;
    }

    Rx = new SIM70XX_CmdResp_t();

    // Get the next item from the queue.
    if(xQueueReceive(Queue, &Rx, portMAX_DELAY) != pdPASS)
    {
        ESP_LOGE(TAG, "     Can not fetch item!");

        Error = SIM70XX_ERR_INVALID_RESPONSE;
        goto SIM7020_Wait_Exit;
    }

    if(Rx->isError)
    {
        ESP_LOGE(TAG, "     Message error!");

        Error = SIM70XX_ERR_FAIL;
        goto SIM7020_Wait_Exit;
    }
    else if(Rx->isTimeout)
    {
        ESP_LOGE(TAG, "     Message timeout!");

        Error = SIM70XX_ERR_TIMEOUT;
        goto SIM7020_Wait_Exit;
    }

SIM7020_Wait_Exit:
    if(p_Response != NULL)
    {
        *p_Response = Rx->Response.c_str();
    }

    if(p_Status != NULL)
    {
        *p_Status = Rx->Status.c_str();
    }

    // Release the allocated memory from the comTask.
    delete Rx;

    return Error;
}

uint32_t SIM70XX_Queue_GetItems(QueueHandle_t Queue)
{
    if(Queue == NULL)
    {
        return SIM70XX_ERR_NOT_INITIALIZED;
    }

    return uxQueueMessagesWaiting(Queue);
}

bool SIM70XX_Queue_Wait(QueueHandle_t Queue, bool* p_Active, uint32_t Timeout, uint32_t Items)
{
    uint32_t Now;
    uint32_t ItemsInQueue;
    uint32_t Timeout_Temp;

    if((Queue == NULL) || (p_Active == NULL))
    {
        return false;
    }

    Timeout_Temp = Timeout * 1000UL;

    Now = SIM70XX_Tools_GetmsTimer();
    do
    {
        ItemsInQueue = uxQueueMessagesWaiting(Queue);
        ESP_LOGD(TAG, "Items in queue: %u", ItemsInQueue);

        if((SIM70XX_Tools_GetmsTimer() - Now) > Timeout_Temp)
        {
            return false;
        }
        else if(*p_Active == false)
        {
            return false;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    } while(ItemsInQueue != Items);

    return true;
}

void SIM70XX_Queue_Reset(QueueHandle_t Queue)
{
    if(Queue == NULL)
    {
        return;
    }

    xQueueReset(Queue);
}

bool SIM70XX_Queue_isEvent(QueueHandle_t Queue, std::string Filter, std::string* p_Event) 
{
    uint32_t ItemsInQueue;

    if(Queue == NULL)
    {
        return false;
    }

    p_Event->clear();

    ItemsInQueue = uxQueueMessagesWaiting(Queue);
    for(uint32_t i = 0; i < ItemsInQueue; i++)
    {
        std::string* Message;

        // Get a new item from the queue to check if the searched event has occured.
        if(xQueuePeek(Queue, &Message, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGD(TAG, "Got event from queue: %s", Message->c_str());

            // Check if the filter match the item.
            if(Message->find(Filter) != std::string::npos)
            {
                *p_Event = *Message;

                // Remove the item from the queue.
                xQueueReceive(Queue, &Message, 0);

                // Delete the item.
                delete Message;

                return true;
            }
        }
    }

    return false;
}