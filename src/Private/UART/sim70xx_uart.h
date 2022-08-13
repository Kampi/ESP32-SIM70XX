 /*
 * sim70xx_uart.h
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

#ifndef SIM70XX_UART_H_
#define SIM70XX_UART_H_

#include <stdint.h>
#include <string>

#include "sim70xx_defs.h"
#include "sim70xx_errors.h"

/** @brief          Initialize the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM70XX_UART_Init(SIM70XX_UART_Conf_t& p_Config);

/** @brief          Deinitialize the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM70XX_UART_Deinit(SIM70XX_UART_Conf_t& p_Config);

/** @brief          Send a command data the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param p_Data   Pointer to data
 *  @param Size     Byte count
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM70XX_UART_Send(SIM70XX_UART_Conf_t& p_Config, const void* p_Data, size_t Size);

/** @brief          Send a command over the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param Command  Command string
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM70XX_UART_SendCommand(SIM70XX_UART_Conf_t& p_Config, std::string Command);

/** @brief          Read a number of bytes from the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param p_Buffer Pointer to data buffer
 *  @param Size     Number of bytes to read
 *  @return         Number of bytes read
 */
size_t SIM70XX_UART_Read(SIM70XX_UART_Conf_t& p_Config, uint8_t* p_Buffer, size_t Size);

/** @brief          Read a character from the serial interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Received character or -1 when no bytes available
 */
int SIM70XX_UART_Read(SIM70XX_UART_Conf_t& p_Config);

/** @brief              Receive a string from the UART interface until a termination character is received.
 *  @param p_Config     Pointer to SIM70XX UART configuration object
 *  @param Terminator   (Optional) Termination character
 *  @param Timeout      (Optional) Timeout in milliseconds
 *  @return             Received string
 */
std::string SIM70XX_UART_ReadStringUntil(SIM70XX_UART_Conf_t& p_Config, char Terminator = '\n', uint32_t Timeout = 1000U);

/** @brief          Receive a string from the UART interface.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Received string
 */
std::string SIM70XX_UART_ReadString(SIM70XX_UART_Conf_t& p_Config);

/** @brief              Flush the UART receive buffer.
 *  @param p_Config     Pointer to SIM70XX UART configuration object
 */
void SIM70XX_UART_Flush(SIM70XX_UART_Conf_t& p_Config);

/** @brief          Get the number of available bytes from the receive buffer.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         Number of bytes available
 */
size_t SIM70XX_UART_Available(SIM70XX_UART_Conf_t& p_Config);

#endif /* SIM70XX_UART_H_ */