 /*
 * sim7080_commands.h
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

#ifndef SIM7080_COMMANDS_H_
#define SIM7080_COMMANDS_H_

#include "sim70xx_commands.h"

/**
 * 
 * Used in SIM7080 device driver.
 * 
 */
#define SIM7080_AT_CFUN_R                                       SIM70XX_CMD("AT+CFUN?", true, 10, 1)
#define SIM7080_AT_CFUN_W(Func)                                 SIM70XX_CMD("AT+CFUN=" + std::to_string(Func), true, 60, 1)
#define SIM7080_AT_CBAND_W(Band)                                SIM70XX_CMD("AT+CBAND=" + std::to_string(Band), false, 10, 1)
#define SIM7080_AT_CBAND_R                                      SIM70XX_CMD("AT+CBAND?", true, 10, 1)
#define SIM7080_AT_CGREG                                        SIM70XX_CMD("AT+CGREG?", true, 10, 1)
#define SIM7080_AT_COPS_W(Command)                              SIM70XX_CMD(Command, false, 300, 1)
#define SIM7080_AT_COPS_R                                       SIM70XX_CMD("AT+COPS=?", true, 300, 1)

/**
 * 
 * Used in SIM7080 device information driver.
 * 
 */
#define SIM7080_AT_CSQ                                          SIM70XX_CMD("AT+CSQ", true, 1, 1)

#endif /* SIM7080_COMMANDS_H_ */