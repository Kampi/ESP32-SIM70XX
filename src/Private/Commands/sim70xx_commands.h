 /*
 * sim70xx_commands.h
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

#ifndef SIM70XX_COMMANDS_H_
#define SIM70XX_COMMANDS_H_

#include <sdkconfig.h>
#include "../Queue/sim70xx_queue.h"

/** @brief  Create a new command object.
 *          An error is generated when no memory is available for allocation.
 */
#define SIM70XX_CREATE_CMD(AT)                                  do                                  \
                                                                {                                   \
                                                                    AT = new SIM70XX_TxCmd_t();     \
                                                                    if(AT == NULL)                  \
                                                                    {                               \
                                                                        return SIM70XX_ERR_NO_MEM;  \
                                                                    }                               \
                                                                } while(0);

/** @brief              SIM70XX command generation macro.
 *  @param AT           AT command
 *  @param HasData      Set to #true to receive data from the command
 *  @param Timeout      Response timeout in seconds
 *  @param Number       Number of response lines to receive
 */
#define SIM70XX_CMD(AT, HasData, TimeOut, Number)               {                                   \
                                                                    .Command = AT,                  \
                                                                    .recData = HasData,             \
                                                                    .Timeout = TimeOut,             \
                                                                    .Lines = Number,                \
                                                                };

/**
 * 
 * Common commands.
 * 
 */
#define SIM70XX_AT                                              SIM70XX_CMD("AT", false, 1, 1)
#define SIM70XX_AT_CBAND_W(Band)                                SIM70XX_CMD("AT+CBAND=" + std::to_string(Band), false, 10, 1)
#define SIM70XX_AT_CFUN_R                                       SIM70XX_CMD("AT+CFUN?", true, 10, 1)
#define SIM70XX_AT_CFUN_W(Func, Reset)                          SIM70XX_CMD("AT+CFUN=" + std::to_string(Func) + "," + std::to_string(Reset), true, 60, 1)
#define SIM70XX_AT_CBAND_R                                      SIM70XX_CMD("AT+CBAND?", true, 10, 1)
#define SIM70XX_AT_CGATT_R                                      SIM70XX_CMD("AT+CGATT?", true, 90, 1)
#define SIM70XX_AT_CPIN_R                                       SIM70XX_CMD("AT+CPIN?", true, 10, 1)

#if(CONFIG_SIMXX_DEV == 7020)
    #include "sim7020_commands.h"
#elif(CONFIG_SIMXX_DEV == 7080)
    #include "sim7080_commands.h"
#endif

#endif /* SIM70XX_COMMANDS_H_ */