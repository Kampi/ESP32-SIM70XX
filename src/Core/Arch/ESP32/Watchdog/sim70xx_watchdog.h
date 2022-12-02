 /*
 * sim70xx_watchdog.h
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

#ifndef SIM70XX_WATCHDOG_H_
#define SIM70XX_WATCHDOG_H_

#include <esp_task_wdt.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "sim70xx_errors.h"

/** @brief          Pause a task.
 *  @param Handle	Task handle
 */
#define SIM70XX_WDT_PAUSE_TASK(Handle)		do											\
											{											\
												SIM70XX_WDT_RemoveHandle(Handle);		\
												vTaskSuspend(Handle);					\
											} while(0);

/** @brief          Continue a task.
 *  @param Handle	Task handle
 */
#define SIM70XX_WDT_CONTINUE_TASK(Handle)	do											\
											{											\
												SIM70XX_WDT_AddHandle(Handle);			\
												vTaskResume(Handle);					\
											} while(0);

/** @brief          Add a task to the task watchdog.
 *  @param Handle	Task handle
 *  @return			SIM70XX_ERR_OK when successful
 */
inline __attribute__((always_inline)) SIM70XX_Error_t SIM70XX_WDT_AddHandle(TaskHandle_t Handle)
{
    if(esp_task_wdt_add(Handle) == ESP_OK)
	{
		return SIM70XX_ERR_OK;
	}

	return SIM70XX_ERR_FAIL;
}

/** @brief          Remove a task from the task watchdog.
 *  @param Handle	Task handle
 *  @return			SIM70XX_ERR_OK when successful
 */
inline __attribute__((always_inline)) SIM70XX_Error_t SIM70XX_WDT_RemoveHandle(TaskHandle_t Handle)
{
    if(esp_task_wdt_delete(Handle) == ESP_OK)
	{
		return SIM70XX_ERR_OK;
	}

	return SIM70XX_ERR_FAIL;
}

/** @brief 	Reset the task watchdog.
 *  @return	SIM70XX_ERR_OK when successful
 */
inline __attribute__((always_inline)) SIM70XX_Error_t SIM70XX_WDT_Reset(void)
{
    if(esp_task_wdt_reset() == ESP_OK)
	{
		return SIM70XX_ERR_OK;
	}

	return SIM70XX_ERR_FAIL;
}

#endif /* SIM70XX_WATCHDOG_H_ */