 /*
 * sim7080_gps_defs.h
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

#ifndef SIM7080_GPS_DEFS_H_
#define SIM7080_GPS_DEFS_H_

#include <string>
#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 GPS port definitions.
 */
typedef enum
{
    SIM7080_GPS_PORT_OFF        = 0,                        /**< Turn off GNSS NMEA data output. */
    SIM7080_GPS_PORT_USB,                                   /**< Turn on GNSS NMEA data output to USB´s NMEA port. */
    SIM7080_GPS_PORT_UART,                                  /**< Turn on GNSS NMEA data output to UART3 port. */
} SIM7080_GPS_Port_t;

/** @brief SIM7080 GPS baudrate definitions.
 */
typedef enum
{
    SIM7080_GPS_BAUD_9600        = 9600,                    /**< Baudrate 9600. */
    SIM7080_GPS_BAUD_19200       = 19200,                   /**< Baudrate 19200. */
    SIM7080_GPS_BAUD_38400       = 38400,                   /**< Baudrate 38400. */
    SIM7080_GPS_BAUD_57600       = 57600,                   /**< Baudrate 57600. */
    SIM7080_GPS_BAUD_115200      = 115200,                  /**< Baudrate 115200. */
} SIM7080_GPS_Baud_t;

/** @brief SIM7080 GPS mode definitions.
 */
typedef enum
{
    SIM7080_GPS_NETWORK_GLONASS     = 0,                    /**< Start GPS and GLONASS constellation. */
    SIM7080_GPS_NETWORK_GALILEO,                            /**< Start GPS and GALILEO constellation. */
    SIM7080_GPS_NETWORK_BEIDOU,                             /**< Start GPS and BEIDOU constellation. */
    SIM7080_GPS_NETWORK_QZSS,                               /**< Start GPS and QZSS constellation. */
} SIM7080_GPS_Network_t;

/** @brief SIM7080 GPS mode definitions.
 */
typedef enum
{
    SIM7080_GPS_MODE_OFF        = 0,                        /**< Turn off GNSS. */
    SIM7080_GPS_MODE_ON_SINGLE,                             /**< Turn on GNSS and get location information once. */
    SIM7080_GPS_MODE_ON_CONT,                               /**< Turn on GNSS and get multiple location information. */
} SIM7080_GPS_Mode_t;

/** @brief SIM7080 GPS power level options definitions.
 */
typedef enum
{
    SIM7080_GPS_PWR_FULL            = 0,                    /**< Use all technologies available to calculate location. */
    SIM7080_GPS_PWR_FULL_LOW,                               /**< Use all low power technologies to calculate location. */
    SIM7080_GPS_PWR_LOW,                                    /**< Use only low and medium power technologies to calculate location. */
} SIM7080_GPS_PwrLevel_t;

/** @brief SIM7080 GPS accuracy options definitions.
 */
typedef enum
{
    SIM7080_GPS_ACCURACY_DEFAULT    = 0,                    /**< Accuracy is not specified, use default. */
    SIM7080_GPS_ACCURACY_LOW,                               /**< Low Accuracy for location is acceptable. */
    SIM7080_GPS_ACCURACY_MEDIUM,                            /**< Medium Accuracy for location is acceptable. */
    SIM7080_GPS_ACCURACY_HIGH,                              /**< Only High Accuracy for location is acceptable. */
} SIM7080_GPS_Accuracy_t;

/** @brief SIM7080 GPS start options definitions.
 */
typedef enum
{
    SIM7080_GPS_START_HOT           = 0,                    /**< Do not delete any data. Perform hot start if the conditions are permitted after starting GNSS. */
    SIM7080_GPS_START_WARM,                                 /**< Delete some related data. Perform warm start if the conditions are permitted after starting GNSS. */
    SIM7080_GPS_START_COLD,                                 /**< Delete all assistance data except almanac data. Enforce cold start after starting GNSS. */
    SIM7080_GPS_START_XTRA_COLD,                            /**< Delete all assistance data except almanac and sv health data. Enforce xtra cold start after starting GNSS. */
    SIM7080_GPS_START_RESET,                                /**< Delete all assistance data. Enforce reset start after starting GNSS. */
} SIM7080_GPS_Start_t;

/** @brief SIM7080 GPS error codes definition.
 */
typedef enum
{
    SIM7080_GPS_ERR_OK                  = 0,                /**< Device disconnected. */
    SIM7080_GPS_ERR_FAILURE,                                /**< General failure. */
    SIM7080_GPS_ERR_CALLBACK,                               /**< Callback is missing. */
    SIM7080_GPS_ERR_PARAM,                                  /**< Invalid parameter. */
    SIM7080_GPS_ERR_ID,                                     /**< ID already exists. */
    SIM7080_GPS_ERR_UNKNOWN,                                /**< ID is unknown. */
    SIM7080_GPS_ERR_STARTED,                                /**< Already started. */
    SIM7080_GPS_ERR_INITIALIZATION,                         /**< Not initialized. */
    SIM7080_GPS_ERR_GEOFENCES,                              /**< Maximum number of geofences reached. */
    SIM7080_GPS_ERR_SUPPORTED,                              /**< Not supported. */
    SIM7080_GPS_ERR_TIMEOUT,                                /**< Timeout when asking single shot. */
    SIM7080_GPS_ERR_GNSS,                                   /**< GNSS engine could not get loaded. */
    SIM7080_GPS_ERR_LOCATION,                               /**< Location module license is disabled. */
    SIM7080_GPS_ERR_INVALID,                                /**< Best available position is invalid. */
} SIM7080_GPS_Error_t;

/** @brief SIM7080 GPS configuration object definition.
 */
typedef struct
{
    SIM7080_GPS_Port_t Port;                                /**< Port used for GPS data output.
                                                                 NOTE: External ports are not supported yet! */
    SIM7080_GPS_Baud_t Baudrate;                            /**< Baudrate used for the GPS data output.
                                                                 NOTE: Only used when \ref SIM7080_GPS_Config_t.Port is set to SIM7080_GPS_PORT_UART. */
    uint16_t Threshold;                                     /**< The threshold for GTP-IoT WWAN fixes to be considered acceptable.
                                                                 The range from 1 to 10000 (Meters). Set to 0 tu use the default value 1000 meters. */
    uint32_t Timeout;                                       /**< Timeout for Single-shot position session. The range from 10000 to 180000 (Milliseconds).
                                                                 Set to 0 to use the default value 30000 milliseconds. */
    SIM7080_GPS_Network_t Network;                          /**< Select the GPS network. */
    SIM7080_GPS_Start_t ADSS;                               /**< Start options. */
    uint8_t NMEA;                                           /**< Bit mask.
                                                                 Each bit enables an NMEA sentence output as follows:
                                                                 Bit 0 GPGSV (GPS satellites in view).
                                                                 Bit 1 GLGSV (GLONASS satellites in view GLONASS fixes only).
                                                                 Bit 2 GAGSV (GALILEO satellites in view).
                                                                 Bit 3 BDGSV/QZGSV (BEIDOU/QZSS satellites in view)
                                                                 Bit 4 GPGSA/GLGSA/GAGSA/BDGSA/QZGSA (1. GPS/2. GLONASS/3. GALILEO/4. BEIDOU/5. QZSS)
                                                                 Bit 5 GNVTG/GPVTG (track made good and ground speed).
                                                                 Bit 6 GNRMC/GPRMC (recommended minimum specific GPS/TRANSIT data).
                                                                 Bit 7 GNGGA/GPGGA (global positioning system fix data). */
    bool Flag;                                              /**< Set to #true to get GPS extra info. */
    bool EnableURC;                                         /**< Set to #true to enable navigation data URC report. */
} SIM7080_GPS_Config_t;

/** @brief SIM7080 GPS information object definition.
 */
typedef struct
{
    std::string Date;                                       /**< Date string from GPS payload.
                                                                 NOTE: Empty when not available. */
    std::string Time;                                       /**< Time string from GPS payload (UTC). */
    float Latitude;                                         /**< Latitude string from GPS payload (±dd.ddddd). */
    float Longitude;                                        /**< Longitude string from GPS payload (±ddd.ddddd). */
    float Accuracy;                                         /**< MSL Accuracy (meters). */
    float Altitude;                                         /**< MSL Altitude (meters). */
    float SeaLevel;                                         /**< MSL Altitude sea level (meters). */
    float Speed;                                            /**< Speed Over Ground (km/h). */
    float Course;                                           /**< Course Over Ground (degrees). */
} SIM7080_GPS_Info_t;

/** @brief SIM7080 GPS data object definition.
 */
typedef struct
{
    std::string Date;                                       /**< Date string from GPS payload.
                                                                 NOTE: Empty when not available. */
    std::string Time;                                       /**< Time string from GPS payload (UTC). */
    float Latitude;                                         /**< Latitude string from GPS payload (±dd.ddddd). */
    float Longitude;                                        /**< Longitude string from GPS payload (±ddd.ddddd). */
    float Accuracy;                                         /**< MSL Accuracy (meters). */
    float Altitude;                                         /**< MSL Altitude (meters). */
    float SeaLevel;                                         /**< MSL Altitude sea level (meters). */
    float Speed;                                            /**< Speed Over Ground (km/h). */
    float Course;                                           /**< Course Over Ground (degrees). */
} SIM7080_GPS_Data_t;

#endif /* SIM7080_GPS_DEFS_H_ */