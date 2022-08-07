 /*
 * sim7080_info_defs.h
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

#ifndef SIM7080_INFO_DEFS_H_
#define SIM7080_INFO_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 device information object definition.
 */
typedef struct
{
    std::string IMEI;                               /**< The International mobile equipment identifier (IMEI) of the attached SIM card.
                                                         NOTE: Managed by the device driver. */
    std::string ICCID;                              /**< The Integrated Circuit Card IDentifier (ICCID) of the attached SIM card.
                                                         NOTE: Managed by the device driver. */
    std::string Firmware;                           /**< Firmware version from the current device.
                                                         NOTE: Managed by the device driver. */
    std::string Manufacturer;                       /**< Manufacturer string.
                                                         NOTE: Managed by the device driver. */
    std::string Model;                              /**< Device model.
                                                         NOTE: Managed by the device driver. */
} SIM7080_Info_t;

/** @brief SIM7080 UE system GSM cell information object definition.
 */
typedef struct
{
    std::string LAC;                        /**< Location Area Code (hexadecimal digits). */
    uint32_t CellID;                        /**< Service-cell Identify. */
    uint32_t RF_Num;                        /**< AFRCN for service-cell. */
    int32_t RxLev;                          /**< */
} SIM7080_GSM_Cell_t;

/** @brief SIM7080 UE system CAT-M or NB-IoT cell information object definition.
 */
typedef struct
{
    std::string TAC;                        /**< Tracing Area Code. */
    uint32_t SCellID;                       /**< Serving Cell ID. */
    std::string Band;                       /**< Frequency Band of active set. */
    uint32_t earfcn;                        /**< E-UTRA absolute radio frequency channel number for searchingCAT-M or NB-IOT cells. */
    uint32_t dlbw;                          /**< Transmission bandwidth configuration of the serving cell onthedownlink. */
    uint32_t ulbw;                          /**< Transmission bandwidth configuration of the serving cell on theuplink. */
    int32_t RSRQ;                           /**< Current reference signal received power. Available for CAT-M or NB-IOT. */
    int32_t RSRP;                           /**< Current reference signal receive quality as measured by L1. */
    int32_t RSSI;                           /**< Current Received signal strength indicator. */
    int32_t RSSNR;                          /**< Average reference signal signal-to-noise ratio of the servingcell.
                                                 The value of SINR can be calculated according to <RSSNR>, the formula is as below:
                                                    SINR = 2 * <RSSNR> - 20
                                                 The range of SINR is from -20 to 30. */
} SIM7080_CAT_Cell_t;

/** @brief SIM7080 UE system information object definition.
 */
typedef struct
{
    SIM7080_GSM_Cell_t GSM;                         /**< GSM cell informations. */
    SIM7080_CAT_Cell_t CAT;                         /**< CAT-M or NB-IoT cell informations. */
    std::string SystemMode;                         /**< System mode. */
    std::string OperationMode;                      /**< UE operation mode. */
    std::string MCC;                                /**< Mobile Country Code (first part of the PLMN code). */
    std::string MNC;                                /**< Mobile Network Code (second part of the PLMN code). */
} SIM7080_UEInfo_t;

#endif /* SIM7080_INFO_DEFS_H_ */