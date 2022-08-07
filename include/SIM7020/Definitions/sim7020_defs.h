 /*
 * sim7020_defs.h
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

#ifndef SIM7020_DEFS_H_
#define SIM7020_DEFS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <string>
#include <vector>
#include <stdint.h>
#include <stdbool.h>
#include <algorithm>

#ifdef CONFIG_SIM70XX_DRIVER_WITH_COAP
    #include "Protocols/sim7020_coap_defs.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_HTTP
    #include "Protocols/sim7020_http_defs.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
    #include "Protocols/sim7020_mqtt_defs.h"
#endif

#ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
    #include "Protocols/sim7020_tcpip_defs.h"
#endif

#include "sim70xx_defs.h"
#include "sim7020_pdp_defs.h"

#include <sdkconfig.h>

/** @brief SIM7020 SIM card status codes definition.
 */
typedef enum
{
    SIM7020_SIM_READY       = 0,                            /**< MT is not pending for any password. */
} SIM7020_SIM_t;

/** @brief Supported frequency bands.
 */
typedef enum
{
    SIM7020_BAND_1          = 0,                            /**< Frequency band 1. */
    SIM7020_BAND_3          = 3,                            /**< Frequency band 3. */
    SIM7020_BAND_5          = 5,                            /**< Frequency band 5. */
    SIM7020_BAND_8          = 8,                            /**< Frequency band 8. */
    SIM7020_BAND_20         = 20,                           /**< Frequency band 20. */
    SIM7020_BAND_28         = 28,                           /**< Frequency band 28. */
} SIM7020_Band_t;

/** @brief Phone functionallity definitions.
 */
typedef enum
{
    SIM7020_FUNC_MIN        = 0,                            /**< Minimum functionality. */
    SIM7020_FUNC_FULL       = 1,                            /**< Full functionality. */
    SIM7020_FUNC_DIS_RF     = 4,                            /**< Disable phone both transmit and receive RF circuits. */
    SIM7020_FUNC_DIS_PHONE  = 7,                            /**< Disable phone SIM only. Transmit and receive circuits still active. */
} SIM7020_Func_t;

/** @brief SIM7020 phone functionallity reset definitions.
 */
typedef enum
{
    SIM7020_RESET_NO        = 0,                            /**< Do not Reset the MT before setting it to <fun> power level. */
    SIM7020_RESET_RESET     = 1,                            /**< Reset the MT before setting it to <fun> power level. */
    SIM7020_RESET_AFTER     = 2,                            /**< Set it to <fun> power level now, and reset the MT after rebooting. */
} SIM7020_Reset_t;

/** @brief SIM7020 AcT definitions.
 */
typedef enum
{
    SIM7020_ACT_NB_IOT      = 9,                            /**< NB-IoT. */
} SIM7020_AcT_t;

/** @brief SIM7020 network registration status codes.
 */
typedef enum
{
    SIM7020_NET_NOT_SEARCHING = 0,                          /**< Not registered, MT is not currently searching an operator to register to. */
    SIM7020_NET_REG_HOME,                                   /**< Registered, home network. */
    SIM7020_NET_NOT_ATTACHED,                               /**< Not registered, but MT is currently trying to attach or searching an operator to register to. */
    SIM7020_NET_DENIED,                                     /**< Registration denied. */
    SIM7020_NET_UNKNOWN,                                    /**< Unknown. */
    SIM7020_NET_ROAMING,                                    /**< Registered, roaming. */
    SIM7020_NET_SMS_HOME,                                   /**< Registered for "SMS only", home network (applicable only when <Act> indicates E-UTRAN). */
    SIM7020_NET_SMS_ROAMING,                                /**< Registered for "SMS only", roaming network (applicable only when <Act> indicates E-UTRAN). */
} SIM7020_NetRegistration_t;

/** @brief SIM7020 network status object definition.
 */
typedef struct
{
    uint32_t sc_earfcn;                                     /**< Indicating the EARFCN for serving cell. Range 0 - 262143. */
    int8_t sc_earfcn_offset;                                /**< Indicating the EARFCN offset for serving cell. */
    uint16_t sc_pci;                                        /**< Indicating the serving cell physical cell ID. Range 0-503. */
    std::string sc_cellid;                                  /**< Four byte (28 bit) cell ID in hexadecimal format for serving cell. */
    int16_t sc_rsrp;                                        /**< Indicating serving cell RSRP value in units of dBm (can be negative value). Available only in RRC-IDLE state. */
    int16_t sc_rsrq;                                        /**< Indicating serving cell RSRQ value in units of dB (can be negative value). Available only in RRC-IDLE state. */
    int16_t sc_rssi;                                        /**< Indicating serving cell RSSI value in units of dBm (can be negative value). Available only in RRC-IDLE state. */
    int8_t sc_snr;                                          /**< Last SNR value for serving cell in units of dB. Available only in RRC-IDLE state. */
    uint8_t sc_band;                                        /**< Current serving cell band. */
    std::string sc_tac;                                     /**< Two byte tracking area code (TAC) in hexadecimal format (e.g. "00C3" equals 195 in decimal). */
    uint8_t sc_ecl;                                         /**< Last Coverage Enhanced Level (CE Level) value for serving cell. Range 0-2. */
    int16_t sc_tx_pwr;                                      /**< Indicating current UE transmit power. Units of cBm Centibels relative to one milliwatt (can be negative value). */
    int16_t sc_re_rsrp;                                     /**< Indicating serving cell RSRP value (the modified) in units of dBm (can be negative value). Available only in RRC-IDLE state. */
} SIM7020_NetState_t;

/** @brief SIM7020 device object.
 */
typedef struct
{
    SIM70XX_UART_Conf_t UART;                               /**< UART configuration object. */
    #ifdef CONFIG_SIM70XX_RESET_USE_HW
        struct
        {
            bool Inverted;						            /**< Set to #true to invert the reset pin. */
            gpio_num_t Pin;						            /**< Reset pin for the module.
                                                                 NOTE: Can be set to -1 when not used. */
        } Reset_Conf;
    #endif
    std::vector<SIM70XX_Operator_t> Operators;              /**< */
    std::string Modes;                                      /**< */
    std::string Formats;                                    /**< */
    SIM7020_PDP_Type_t PDP_Type;                            /**< The PDP type used by the device. */
    struct
    {
        std::string IP;                                     /**< Device IP address. */
        std::string Subnet;                                 /**< */
        std::string APN;                                    /**< A string parameter which is a logical name that was used to select the GGSN or the external packet data network. */
        uint8_t CID;                                        /**< A numeric parameter which specifies a particular primary PDP \n
                                                                 context definition. The parameter is local to the TE-UE interface and is \n
                                                                 used in other PDP context-related commands. */
        uint8_t Baerer;                                     /**< A numeric parameter which identifies the bearer, EPS Bearer in EPS and NSAPI in UMTS/GPRS. */
    } PDP;
    struct
    {
        SIM7020_NetRegistration_t Status;                   /**< Network status. */
        SIM7020_Func_t Functionality;                       /**< Current device functionality. */
    } Connection;
    #ifdef CONFIG_SIM70XX_DRIVER_WITH_TCPIP
        struct
        {
            std::vector<SIM7020_TCP_Socket_t*> Sockets;     /**< List with pointer to active TCP sockets.
                                                                 NOTE: Managed by the device driver. */
        } TCP;
    #endif
    #ifdef CONFIG_SIM70XX_DRIVER_WITH_HTTP
        struct
        {
            std::vector<SIM7020_HTTP_Socket_t*> Sockets;    /**< List with pointer to active HTTP sockets.
                                                                 NOTE: Managed by the device driver. */
        } HTTP;
    #endif
    #ifdef CONFIG_SIM70XX_DRIVER_WITH_MQTT
        struct
        {
            std::vector<SIM7020_MQTT_Socket_t*> Sockets;    /**< List with pointer to active MQTT sockets.
                                                                 NOTE: Managed by the device driver. */
            QueueHandle_t SubQueue;                         /**< Subscribe event queue.
                                                                 NOTE: Managed by the device driver. */
            uint32_t SubTopics;                             /**< Subscribe counter.
                                                                 NOTE: Managed by the device driver. */
        } MQTT;
    #endif
    #ifdef CONFIG_SIM70XX_DRIVER_WITH_COAP
        struct
        {
            std::vector<SIM7020_CoAP_Socket_t*> Sockets;    /**< List with pointer to active CoAP sockets.
                                                                 NOTE: Managed by the device driver. */
        } CoAP;
    #endif
    struct
    {
        QueueHandle_t RxQueue;                              /**< Message receive (Module -> ESP32) queue.
                                                                 NOTE: Managed by the device driver. */
        QueueHandle_t TxQueue;                              /**< Message transmit (ESP32 -> Module) queue.
                                                                 NOTE: Managed by the device driver. */
        QueueHandle_t EventQueue;                           /**< Asynchronous event queue.
                                                                 NOTE: Managed by the device driver. */
        bool isInitialized;                                 /**< #true when the module is initialized and ready to use.
                                                                 NOTE: Managed by the device driver. */
        bool isPSM;                                         /**< #true when the module has entered PSM.
                                                                 NOTE: Managed by the device driver. */
        bool isPSMEvent;                                    /**< #true when PSM event notifications are enabled.
                                                                 NOTE: Managed by the device driver. */
        bool isActive;                                      /**< #true when the device is active and ready to use.
                                                                 NOTE: Managed by the device driver. */
        TaskHandle_t TaskHandle;                            /**< Handle of the receive task.
                                                                 NOTE: Managed by the device driver. */
    } Internal;
} SIM7020_t;

/** @brief SIM7020 device configuration object.
 */
typedef struct
{
    SIM70XX_UART_Conf_t UART;                               /**< UART configuration object. */
    #ifdef CONFIG_SIM70XX_RESET_USE_HW
        struct
        {
            bool Inverted;						            /**< Set to #true to invert the reset pin. */
            gpio_num_t Pin;						            /**< Reset pin for the module.
                                                                 NOTE: Can be set to -1 when not used. */
        } Reset_Conf;
    #endif
    SIM7020_Band_t Band;                                    /**< Selected frequency band. */
    SIM70XX_APN_t APN;                                      /**< APN configuration object. */
    SIM70XX_OpForm_t OperatorFormat;                        /**< Format for the selected operator. */
    std::string Operator;                                   /**< Selected operator. */
} SIM7020_Config_t;

#endif /* SIM7020_DEFS_H_ */