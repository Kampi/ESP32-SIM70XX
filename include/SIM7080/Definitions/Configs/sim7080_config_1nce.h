 /*
 * sim7080_config_1nce.h
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

#ifndef SIM7080_CONFIG_1NCE_H_
#define SIM7080_CONFIG_1NCE_H_

/** @brief          Default NB-IoT configuration for using the SIM7080 module with the 1nce APN.
 *  @param Serial   Serial interface that should be used by the driver
 *  @param Baud     Baud rate that should be used by the driver
 *  @param Pin_Rx   Rx pin for the serial interface
 *  @param Pin_Tx   Tx pin for the serial interface
 */
#define SIM70XX_DEFAULT_CONF_1NCE(Serial, Baud, Pin_Rx, Pin_Tx)     {                                                       \
                                                                        .UART = {                                           \
                                                                            .Rx = Pin_Rx,                                   \
                                                                            .Tx = Pin_Tx,                                   \
                                                                            .Interface = Serial,                            \
                                                                            .Lock = NULL,                                   \
                                                                            .Baudrate = Baud,                               \
                                                                            .isInitialized = false,                         \
                                                                        },                                                  \
                                                                        .Band = SIM7080_BAND_ALL,                           \
                                                                        .APN = {                                            \
                                                                            .Name = "iot.1nce.net",                         \
                                                                            .Username = "",                                 \
                                                                            .Password = "",                                 \
                                                                        },                                                  \
                                                                        .OperatorFormat = SIM_FORM_NUMERIC,                 \
                                                                        .Operator = "26201",                                \
                                                                        .NetMode = SIM7080_NETMODE_LTE,                     \
                                                                        .Mode = SIM7080_MODE_NB,                            \
                                                                        .Bandlist = std::vector<uint8_t>(),                 \
                                                                    };

#endif /* SIM7080_CONFIG_1NCE_H_ */