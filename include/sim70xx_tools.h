 /*
 * sim70xx_tools.h
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

#ifndef SIM70XX_TOOLS_H_
#define SIM70XX_TOOLS_H_

#include <esp_attr.h>

#include <string>
#include <stdint.h>
#include <stdbool.h>

#include "sim70xx_defs.h"
#include "sim70xx_errors.h"

/** @brief          Remove a line ending (\n and \r) from the input string.
 *  @param String   Input string
 */
#define SIMXX_TOOLS_REMOVE_LINEEND(String)                  do                                                                                  \
                                                            {                                                                                   \
                                                                String.erase(std::remove(String.begin(), String.end(), '\n'), String.end());    \
                                                                String.erase(std::remove(String.begin(), String.end(), '\r'), String.end());    \
                                                            } while(0);

/** @brief  Get the version number of the SIM70XX library.
 *  @return Library version
 */
const inline __attribute__((always_inline)) std::string SIM70XX_LibVersion(void)
{
    #if((defined SIM70XX_LIB_MAJOR) && (defined SIM70XX_LIB_MINOR) && (defined SIM70XX_LIB_BUILD))
        return std::string(STRINGIFY(SIM70XX_LIB_MAJOR)) + "." + std::string(STRINGIFY(SIM70XX_LIB_MINOR)) + "." + std::string(STRINGIFY(SIM70XX_LIB_BUILD));
    #else
        return "<Not defined>";
    #endif
}

/** @brief          Convert a buffer into a hexadecimal ASCII string.
 *  @param p_Buffer Pointer to data buffer
 *  @param Length   Buffer length
 *                  NOTE: Length must be a power of 2!
 *  @param p_Hex    Pointer to hexadecimal ASCII string
 */
void SIM70XX_Tools_ASCII2Hex(const void* const p_Buffer, uint32_t Length, std::string* const p_Hex);

/** @brief          Convert a hexadecimal ASCII string into a buffer.
 *  @param Hex      Hex string
 *  @param p_Buffer Pointer to data buffer
 */
void SIM70XX_Tools_Hex2ASCII(std::string Hex, uint8_t* const p_Buffer);

/** @brief          Convert a buffer into a Base64 string.
 *  @param p_Buffer Input buffer
 *  @param Length   Input buffer length
 *  @param p_Base64 Base64 output string
 *  @return         #true when successful
 */
bool SIM70XX_Tools_ToBase64(const void* const p_Buffer, uint32_t Length, std::string* const p_Base64);

/** @brief  Get the milliseconds from the ESP timer.
 *  @return Milliseconds since boot
 */
unsigned long IRAM_ATTR SIM70XX_Tools_GetmsTimer(void);

/** @brief          Enable the module.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @param Cycles   (Optional) Number of activation cycles
 *  @return         #true when the module is enabled
 */
bool SIM70XX_Tools_EnableModule(SIM70XX_UART_Conf_t& p_Config, uint8_t Cycles = 20);

/** @brief Disable the module.
 */
void SIM70XX_Tools_DisableModule(void);

/** @brief          Check if the SIM70XX module is active.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         #true when the module is active
 */
bool SIM70XX_Tools_isActive(SIM70XX_UART_Conf_t& p_Config);

/** @brief Perform a reset of the module.
 */
void SIM70XX_Tools_ResetModule(void);

/** @brief          Disable the echo mode of the SIM70XX module.
 *  @param p_Config Pointer to SIM70XX UART configuration object
 *  @return         SIM70XX_ERR_OK when successful
 */
SIM70XX_Error_t SIM70XX_Tools_DisableEcho(SIM70XX_UART_Conf_t& p_Config);

/** @brief              Get a substring from the input string. The substring is delimited by the given delimiter.
 *  @param p_Input      Pointer to input string
 *                      NOTE: The input string will be modified!
 *  @param Delimiter    Substring delimiter
 *  @return             Substring
 */
std::string SIM70XX_Tools_SubstringSplitErase(std::string* p_Input, std::string Delimiter = ",");

#endif /* SIM70XX_TOOLS_H_ */