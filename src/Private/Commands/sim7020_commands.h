 /*
 * sim7020_commands.h
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

#ifndef SIM7020_COMMANDS_H_
#define SIM7020_COMMANDS_H_

#include "sim70xx_commands.h"

/**
 * 
 * Used in SIM7020 device driver.
 * 
 */
#define SIM7020_AT_MCGDEFCONT(Command)                          SIM70XX_CMD(Command, false, 60, 1)
#define SIM7020_AT_COPS_W(Command)                              SIM70XX_CMD(Command, false, 300, 1)
#define SIM7020_AT_COPS_R                                       SIM70XX_CMD("AT+COPS=?", true, 300, 1)
#define SIM7020_AT_IPR_W(Baud)                                  SIM70XX_CMD("AT+IPR=" + std::to_string(Baud), false, 10, 1)

/**
 * 
 * Used in SIM7020 PDP context device driver.
 * 
 */
#define SIM7020_AT_CGACT_W(ID, Enable)                          SIM70XX_CMD("AT+CGACT=" + std::to_string(Enable) + "," + std::to_string(ID), false, 10, 1)
#define SIM7020_AT_CGACT_R                                      SIM70XX_CMD("AT+CGACT?", true, 10, 1)
#define SIM7020_AT_CGDEL(ID)                                    SIM70XX_CMD("AT+CGDEL=" + std::to_string(ID), true, 10, 1)
#define SIM7020_AT_CGDCONT_R                                    SIM70XX_CMD("AT+CGDCONT?", true, 10, 1)

/**
 * 
 * Used in SIM7020 device information driver.
 * 
 */
#define SIM7020_AT_CGMI                                         SIM70XX_CMD("AT+CGMI", true, 1, 1)
#define SIM7020_AT_CGMM                                         SIM70XX_CMD("AT+CGMM", true, 1, 1)
#define SIM7020_AT_CGMR                                         SIM70XX_CMD("AT+CGMR", true, 1, 1)
#define SIM7020_AT_CGOI                                         SIM70XX_CMD("AT+CGOI", true, 1, 1)
#define SIM7020_AT_CGSN                                         SIM70XX_CMD("AT+CGSN", true, 1, 1)
#define SIM7020_AT_CIMI                                         SIM70XX_CMD("AT+CIMI", true, 1, 1)
#define SIM7020_AT_CCID                                         SIM70XX_CMD("AT+CCID", true, 1, 1)
#define SIM7020_AT_CGMR                                         SIM70XX_CMD("AT+CGMR", true, 1, 1)
#define SIM7020_AT_CGCONTRDP_W(ID)                              SIM70XX_CMD("AT+CGCONTRDP=" + std::to_string(ID), true, 60, 1)
#define SIM7020_AT_CGCONTRDP                                    SIM70XX_CMD("AT+CGCONTRDP", true, 300, 1)
#define SIM7020_AT_CGREG                                        SIM70XX_CMD("AT+CGREG?", true, 10, 1)
#define SIM7020_AT_CSQ                                          SIM70XX_CMD("AT+CSQ", true, 1, 1)
#define SIM7020_AT_CENG_R                                       SIM70XX_CMD("AT+CENG?", true, 1, 1)
#define SIM7020_AT_CENG_W(Mode)                                 SIM70XX_CMD("AT+CENG=" + std::to_String(Mode), false, 1, 1)

/**
 * 
 * Used in SIM7020 NVRAM driver.
 * 
 */
#define SIM7020_AT_CNVMR(Key)                                   SIM70XX_CMD("AT+CNVMR=\"" + Key + "\"", true, 10, 1)
#define SIM7020_AT_CNVMW(Key, Payload, Length)                  SIM70XX_CMD("AT+CNVMW=\"" + Key + "\",\"" + Payload + "\"," + std::to_string(Length), true, 10, 1)
#define SIM7020_AT_CNVMGET                                      SIM70XX_CMD("AT+CNVMGET", true, 10, 0)
#define SIM7020_AT_CNVMIVD(Key)                                 SIM70XX_CMD("AT+CNVMIVD=\"" + Key + "\"", true, 10, 1)

/**
 * 
 * Used in SIM7020 Power Management driver.
 * 
 */
#define SIM7020_AT_RETENTION(Enable)                            SIM70XX_CMD("AT+RETENTION=" + std::to_string(Enable), false, 10, 1)
#define SIM7020_AT_CPSMS_DIS(Mode)                              SIM70XX_CMD("AT+CPSMS=" + std::to_string(Mode), false, 1, 1)
#define SIM7020_AT_CPSMS_EN(TAU, Active)                        SIM70XX_CMD("AT+CPSMS=1,,,\"" + std::to_string(TAU) + "\",\"" + std::to_string(Active) + "\"", false, 1, 1)
#define SIM7020_AT_CPSMSTATUS_W(Enable)                         SIM70XX_CMD("AT+CPSMSTATUS=" + std::to_string(Enable), false, 1, 1)
#define SIM7020_AT_CPSMSTATUS_R                                 SIM70XX_CMD("AT+CPSMSTATUS?", true, 1, 1)

/**
 * 
 * Used in SIM7020 TCP driver.
 * 
 */
#define SIM7020_AT_CDNSGIP(Host)                                SIM70XX_CMD("AT+CDNSGIP=\"" + Host + "\"", false, 1, 1)
#define SIM7020_AT_CIPPING(Command)                             SIM70XX_CMD(Command, false, 60, 1)
#define SIM7020_AT_CSOC(Command)                                SIM70XX_CMD(Command, true, 60, 1)
#define SIM7020_AT_CSOCON(ID, Port, Address)                    SIM70XX_CMD("AT+CSOCON=" + std::to_string(ID) + "," + std::to_string(Port) + ",\"" + Address + "\"", false, 60, 1)
#define SIM7020_AT_CCSOSEND_BYTES(ID, Length, Data)             SIM70XX_CMD("AT+CSOSEND=" + std::to_string(ID) + "," + std::to_string(Length) + "," + Data, false, 60, 1)
#define SIM7020_AT_CCSOSEND_STRING(ID, Data)                    SIM70XX_CMD("AT+CSOSEND=" + std::to_string(ID) + ",0,\"" + Data + "\"", false, 60, 1)
#define SIM7020_AT_CSOCL(ID)                                    SIM70XX_CMD("AT+CSOCL=" + std::to_string(ID), false, 60, 1)

/**
 * 
 * Used in SIM7020 SNTP driver.
 * 
 */
#define SIM7020_AT_CSNTPSTART(Server, Zone)                     SIM70XX_CMD("AT+CSNTPSTART=\"" + Server + "\",\"" + Zone + "\"", false, 60, 1)
#define SIM7020_AT_CSNTPSTOP                                    SIM70XX_CMD("AT+CSNTPSTOP", false, 60, 1)

/**
 * 
 * Used in SIM7020 HTTP(S) driver.
 * 
 */
#define SIM7020_AT_CHTTPCREATE(Host)                            SIM70XX_CMD("AT+CHTTPCREATE=" + Host, true, 60, 1)
#define SIM7020_AT_CHTTCON(ID)                                  SIM70XX_CMD("AT+CHTTPCON=" + std::to_string(ID), false, 60, 1)
#define SIM7020_AT_CHTTPSEND(Command)                           SIM70XX_CMD(Command, false, 120, 1)
#define SIM7020_AT_CHTTPSENDEXT(Command)                        SIM70XX_CMD(Command, false, 120, 1)
#define SIM7020_AT_CHTTPDISCON(ID)                              SIM70XX_CMD("AT+CHTTPDISCON=" + std::to_string(ID), false, 10, 1)
#define SIM7020_AT_CHTTPDESTROY(ID)                             SIM70XX_CMD("AT+CHTTPDESTROY=" + std::to_string(ID), false, 10, 1)

/**
 * 
 * Used in SIM7020 MQTT driver.
 * 
 */
#define SIM7020_AT_CMQNEW(Command)                              SIM70XX_CMD(Command, true, 60, 1)
#define SIM7020_AT_CMQCON(Command)                              SIM70XX_CMD(Command, false, 60, 1)
#define SIM7020_AT_CMQPUB(Command)                              SIM70XX_CMD(Command, false, 60, 1)
#define SIM7020_AT_CMQSUB(ID, Topic, QoS)                       SIM70XX_CMD("AT+CMQSUB=" + std::to_string(ID) + ",\"" + Topic + "\"," + std::to_string(QoS), false, 10, 1)
#define SIM7020_AT_CMQUNSUB(ID, Topic)                          SIM70XX_CMD("AT+CMQUNSUB=" + std::to_string(ID) + ",\"" + Topic + "\"", false, 10, 1)
#define SIM7020_AT_CMQDISCON(ID)                                SIM70XX_CMD("AT+CMQDISCON=" + std::to_string(ID), false, 10, 1)

/**
 * 
 * Used in SIM7020 CoAP driver.
 * 
 */
#define SIM7020_AT_CCOAPNEW(Command)                            SIM70XX_CMD(Command, true, 60, 1)
#define SIM7020_AT_CCOAPCSEND(Command)                          SIM70XX_CMD(Command, false, 60, 1)
#define SIM7020_AT_CCOAPDEL(ID)                                 SIM70XX_CMD("AT+CCOAPDEL=" + std::to_string(ID), false, 10, 1)

#endif /* SIM7020_COMMANDS_H_ */