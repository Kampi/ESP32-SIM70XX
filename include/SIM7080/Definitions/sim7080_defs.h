 /*
 * sim7080_defs.h
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

#ifndef SIM7080_DEFS_H_
#define SIM7080_DEFS_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <string>
#include <vector>
#include <stdint.h>
#include <stdbool.h>
#include <algorithm>

#include <sdkconfig.h>

#include "sim70xx_defs.h"

/** @brief SIM7080 SIM card status codes definition.
 */
typedef enum
{
    SIM7080_SIM_READY       = 0,                            /**< MT is not pending for any password. */
    SIM7080_SIM_WAIT_PIN,                                   /**< MT is waiting SIM PIN to be given. */
    SIM7080_SIM_WAIT_PUK,                                   /**< MT is waiting for SIM PUK to be given. */
    SIM7080_SIM_WAIT_PH_PIN,                                /**< ME is waiting for phone to SIM card (antitheft). */
    SIM7080_SIM_WAIT_PH_PUK,                                /**< ME is waiting for SIM PUK (antitheft). */
    SIM7080_SIM_WAIT_NET_PIN,                               /**< ME is waiting network personalization password to be given. */
    SIM7080_SIM_WAIT_PIN2,                                  /**< PIN2, e.g. for editing the FDN book possible only if preceding Command was acknowledged with +CME ERROR:17. */
    SIM7080_SIM_WAIT_PUK2,                                  /**< Possible only if preceding Command was acknowledged with error +CME ERROR: 18. */
} SIM7080_SIM_t;

/** @brief SIM7080 preferred network modes.
 */
typedef enum
{
    SIM7080_NETMODE_AUTO    = 2,                            /**< Automatic mode selection. */
    SIM7080_NETMODE_GSM     = 13,                           /**< GSM only mode. */
    SIM7080_NETMODE_LTE     = 38,                           /**< LTE mode only. */
    SIM7080_NETMODE_GSM_LTE = 51,                           /**< GSM and LTE mode only. */
} SIM7080_NetMode_t;

/** @brief SIM7080 preferred selections between CAT-M and NB-IoT
 */
typedef enum
{
    SIM7080_SEL_CAT         = 1,                            /**< Select CAT-M. */
    SIM7080_SEL_NB          = 2,                            /**< Select NB-IoT. */
    SIM7080_SEL_BOTH        = 3,                            /**< Select CAT-M and NB-IoT. */
} SIM7080_Sel_t;

/** @brief SIM7080 supported frequency bands.
 */
typedef enum
{
    SIM7080_BAND_EGSM       = 253,                          /**< EGSM mode. */
    SIM7080_BAND_DCS        = 254,                          /**< DCS mode. */
    SIM7080_BAND_ALL        = 255,                          /**< All band mode. */
} SIM7080_Band_t;

/** @brief SIM7080 phone functionallity definitions.
 */
typedef enum
{
    SIM7080_FUNC_MIN        = 0,                            /**< Minimum functionality. */
    SIM7080_FUNC_FULL       = 1,                            /**< Full functionality. */
    SIM7080_FUNC_DIS_RF     = 4,                            /**< Disable phone both transmit and receive RF circuits. */
    SIM7080_FUNC_TEST       = 5,                            /**< Factory Test mode. */
    SIM7080_FUNC_RESET      = 6,                            /**< Reset. */
    SIM7080_FUNC_OFFLINE    = 7,                            /**< Offline mode. */
} SIM7080_Func_t;

/** @brief SIM7080 network registration status codes.
 */
typedef enum
{
    SIM7080_NET_NOT_SEARCHING = 0,                          /**< Not registered, MT is not currently searching an operator to
                                                                 register to. The GPRS service is disabled, the UE is allowed to attach
                                                                 for GPRS if requested by the user. */
    SIM7080_NET_REG_HOME,                                   /**< Registered, home network. */
    SIM7080_NET_NOT_ATTACHED,                               /**< Not registered, but MT is currently trying to attach or searching an
                                                                 operator to register to. The GPRS service is enabled, but an allowable
                                                                 PLMN is currently not available. The UE will start a GPRS attach as
                                                                 soon as an allowable PLMN is available.
                                                                 3 Registration denied, The GPRS service */
    SIM7080_NET_UNKNOWN,                                    /**< Unknown. */
    SIM7080_NET_ROAMING,                                    /**< Registered, roaming. */
} SIM7080_NetRegistration_t;

/** @brief SIM7080 AcT definitions.
 */
typedef enum
{
    SIM7080_ACT_USER_GSM    = 0,                            /**< User-specific GSM. */
    SIM7080_ACT_GSM         = 1,                            /**< GSM compact. */
    SIM7080_ACT_EGPRS       = 3,                            /**< GSM EGPRS. */
    SIM7080_ACT_USER_LTE_M1 = 7,                            /**< User-specific LTE M1 A GB. */
    SIM7080_ACT_USER_LTE_NB = 9,                            /**< User-specific LTE NB S1. */
} SIM7080_AcT_t;

/** @brief SIM7080 device object.
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
    struct
    {
        int8_t RSSI;                                        /**< Network RSSI value in dBm. */
        uint8_t RXQual;                                     /**< RxQUAL values in the table in GSM 05.08 [20] subclause 7.2.4. */
        SIM7080_NetRegistration_t Status;                   /**< Network status. */
        SIM7080_Func_t Functionality;                       /**< Current device functionality. */
    } Connection;
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
} SIM7080_t;

/** @brief SIM7080 device configuration object.
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
    SIM7080_Band_t Band;                                    /**< Selected frequency band. */
    SIM70XX_APN_t APN;                                      /**< APN configuration object. */
    SIM70XX_OpForm_t OperatorFormat;                        /**< Format for the selected operator. */
    std::string Operator;                                   /**< Selected operator. */
    SIM7080_NetMode_t Mode;                                 /**< Preferred network mode selection. */
    SIM7080_Sel_t Selection;                                /**< Preffered selection between CAT-M and NB-IoT. */
} SIM7080_Config_t;

#endif /* SIM7080_DEFS_H_ */