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
#define SIM7080_AT_CNMP_W(Mode)                                 SIM70XX_CMD("AT+CNMP=" + std::to_string(Mode), false, 10, 1)
#define SIM7080_AT_CNMP_R                                       SIM70XX_CMD("AT+CNMP?", true, 10, 1)
#define SIM7080_AT_CMNB_W(Sel)                                  SIM70XX_CMD("AT+CMNB=" + std::to_string(Sel), false, 10, 1)
#define SIM7080_AT_CMNB_R                                       SIM70XX_CMD("AT+CMNB?", true, 10, 1)
#define SIM7080_AT_CGREG                                        SIM70XX_CMD("AT+CGREG?", true, 10, 1)
#define SIM7080_AT_COPS_W(Command)                              SIM70XX_CMD(Command, false, 300, 1)
#define SIM7080_AT_COPS_R                                       SIM70XX_CMD("AT+COPS?", true, 300, 1)
#define SIM7080_AT_CGDCONT_W(Command)                           SIM70XX_CMD(Command, false, 10, 1)
#define SIM7080_AT_CSQ                                          SIM70XX_CMD("AT+CSQ", true, 1, 1)
#define SIM70XX_AT_CBANDCFG_R                                   SIM70XX_CMD("AT+CBANDCFG?", true, 1, 2)
#define SIM70XX_AT_CBANDCFG_W(Mode, Bandlist)                   SIM70XX_CMD("AT+CBANDCFG=" + Mode + "," + Bandlist, false, 10, 1)
#define SIM7080_AT_CNCFG_W(Command)                             SIM70XX_CMD("AT+CNCFG=" + Command, false, 1, 1)
#define SIM7080_AT_CNACT_W(PDP, Action)                         SIM70XX_CMD("AT+CNACT=" + std::to_string(PDP) + "," + std::to_string(Action), true, 300, 1)
#define SIM7080_AT_CNACT_R                                      SIM70XX_CMD("AT+CNACT?", true, 10, 4)

/**
 * 
 * Used in SIM7080 device information driver.
 * 
 */
#define SIM7080_AT_CGMI                                         SIM70XX_CMD("AT+CGMI", true, 1, 1)
#define SIM7080_AT_CGMM                                         SIM70XX_CMD("AT+CGMM", true, 1, 1)
#define SIM7080_AT_CGMR                                         SIM70XX_CMD("AT+CGMR", true, 1, 1)
#define SIM7080_AT_CGOI                                         SIM70XX_CMD("AT+CGOI", true, 1, 1)
#define SIM7080_AT_CGSN                                         SIM70XX_CMD("AT+CGSN", true, 1, 1)
#define SIM7080_AT_CIMI                                         SIM70XX_CMD("AT+CIMI", true, 1, 1)
#define SIM7080_AT_CCID                                         SIM70XX_CMD("AT+CCID", true, 1, 1)
#define SIM7080_AT_CGMR                                         SIM70XX_CMD("AT+CGMR", true, 1, 1)
#define SIM7080_AT_CGCONTRDP_W(ID)                              SIM70XX_CMD("AT+CGCONTRDP=" + std::to_string(ID), true, 60, 1)
#define SIM7080_AT_CGCONTRDP                                    SIM70XX_CMD("AT+CGCONTRDP", true, 300, 1)
#define SIM7080_AT_CGREG                                        SIM70XX_CMD("AT+CGREG?", true, 10, 1)
#define SIM7080_AT_CSQ                                          SIM70XX_CMD("AT+CSQ", true, 1, 1)
#define SIM7080_AT_CPSI                                         SIM70XX_CMD("AT+CPSI?", true, 1, 1)
#define SIM7080_AT_CENG_R                                       SIM70XX_CMD("AT+CENG?", true, 1, 1)
#define SIM7080_AT_CENG_W(Mode)                                 SIM70XX_CMD("AT+CENG=" + std::to_String(Mode), false, 1, 1)

/**
 * 
 * Used in SIM7080 TCP driver.
 * 
 */
#define SIM7080_AT_SNPING4(Host, Retries, Size, Timeout, Items) SIM70XX_CMD("AT+SNPING4=\"" + Host + "\"," + std::to_string(Retries) + "," + std::to_string(Size) + "," + std::to_string(Timeout), true, 60, Items)
#define SIM7080_AT_SNPING6(Host, Retries, Size, Timeout, Items) SIM70XX_CMD("AT+SNPING6=\"" + Host + "\"," + std::to_string(Retries) + "," + std::to_string(Size) + "," + std::to_string(Timeout), true, 60, Items)
#define SIM7080_AT_CDNSGIP(Host)                                SIM70XX_CMD("AT+CDNSGIP=\"" + Host + "\"", false, 1, 1)
#define SIM7080_AT_CAOPEN(ID, PDP, Type, Address, Port)         SIM70XX_CMD("AT+CAOPEN=" + std::to_string(ID) + "," + std::to_string(PDP) + "," + "\"" + Type + "\",\"" + Address + "\"," + std::to_string(Port), true, 0, 1)
#define SIM7080_AT_CASEND(ID, Size)                             SIM70XX_CMD("AT+CASEND=" + std::to_string(ID) + "," + std::to_string(Size), false, 10, 1)
#define SIM7080_AT_CARECV(ID, Size)                             SIM70XX_CMD("AT+CARECV=" + std::to_string(ID) + "," + std::to_string(Size), true, 10, 1)
#define SIM7020_AT_CACLOSE(ID)                                  SIM70XX_CMD("AT+CACLOSE=" + std::to_string(ID), false, 1, 1)

/**
 * 
 * Used in SIM7080 NTP driver.
 * 
 */
#define SIM7020_AT_CNTP_W(Server, TimeZone, ID, Mode)           SIM70XX_CMD("AT+CNTP=\"" + Server + "\"," + std::to_string(TimeZone) + "," + std::to_string(ID) + "," + std::to_string(Mode), false, 60, 1)
#define SIM7020_AT_CNTP                                         SIM70XX_CMD("AT+CNTP", true, 60, 1)

/**
 * 
 * Used in SIM7080 file system driver.
 * 
 */
#define SIM7080_AT_CFSINIT                                      SIM70XX_CMD("AT+CFSINIT", false, 10, 1)
#define SIM7080_AT_CFSWFILE(Path, Name, Mode, Size)             SIM70XX_CMD("AT+CFSWFILE=" + std::to_string(Path) + ",\"" + Name + "\"," + std::to_string(Mode) + "," + std::to_string(Size) + ",100", false, 1, 1)
#define SIM7080_AT_CFSRFILE(Path, Name, Mode, Size, Position)   SIM70XX_CMD("AT+CFSRFILE=" + std::to_string(Path) + ",\"" + Name + "\"," + std::to_string(Mode) + "," + std::to_string(Size) + "," + std::to_string(Position), false, 1, 1)
#define SIM7080_AT_CFSGFRS                                      SIM70XX_CMD("AT+CFSGFRS?", true, 1, 1)
#define SIM7080_AT_CFSGFIS(Path, Name)                          SIM70XX_CMD("AT+CFSGFIS=" + std::to_string(Path) + ",\"" + Name + "\"", true, 1, 1)
#define SIM7080_AT_CFSDFILE(Path, Name)                         SIM70XX_CMD("AT+CFSDFILE=" + std::to_string(Path) + ",\"" + Name + "\"", false, 1, 1)
#define SIM7080_AT_CFSREN(Path, Old, New)                       SIM70XX_CMD("AT+CFSREN=" + std::to_string(Path) + ",\"" + Old + "\",\"" + New + "\"", false, 1, 1)
#define SIM7080_AT_CFSTERM                                      SIM70XX_CMD("AT+CFSTERM", false, 1, 1)

/**
 * 
 * Used in SIM7080 E-Mail driver.
 * 
 */
#define SIM7080_AT_EMAILCID(ID)                                 SIM70XX_CMD("AT+EMAILCID=" + std::to_string(ID), false, 1, 1)
#define SIM7080_AT_EMAILTO(Timeout)                             SIM70XX_CMD("AT+EMAILTO=" + std::to_string(Timeout), false, 1, 1)
#define SIM7080_AT_SMTPSRV(Address, Port)                       SIM70XX_CMD("AT+SMTPSRV=\"" + Address + "\"," + std::to_string(Port), false, 1, 1)
#define SIM7080_AT_SMTPAUTH(User, Password)                     SIM70XX_CMD("AT+SMTPAUTH=1,\"" + User + "\",\"" + Password + "\"", false, 1, 1)
#define SIM7080_AT_SMTPFROM(Address, Name)                      SIM70XX_CMD("AT+SMTPFROM=\"" + Address + "\",\"" + Name + "\"", false, 1, 1)
#define SIM7080_AT_SMTPRCPT(Type, Address, Name)                SIM70XX_CMD("AT+SMTPRCPT=" + std::to_string(Type) + ",0,\"" + Address + "\",\"" + Name + "\"", false, 1, 1)
#define SIM7080_AT_SMTPSUB(Subject)                             SIM70XX_CMD("AT+SMTPSUB=" + Subject, false, 1, 1)
#define SIM7080_AT_SMTPBODY(Size)                               SIM70XX_CMD("AT+SMTPBODY=" + std::to_string(Size), false, 1, 1)
#define SIM7080_AT_SMTPSEND                                     SIM70XX_CMD("AT+SMTPSEND", false, 300, 1)

#endif /* SIM7080_COMMANDS_H_ */