 /*
 * sim7080_fs_defs.h
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

#ifndef SIM7080_FS_DEFS_H_
#define SIM7080_FS_DEFS_H_

#include <stdint.h>
#include <stdbool.h>

/** @brief SIM7080 file system path definitions.
 */
typedef enum
{
    SIM7080_FS_PATH_APP         = 0,                /**< Use the path /custapp. */
    SIM7080_FS_PATH_FOTA,                           /**< Use the path /fota. */
    SIM7080_FS_PATH_DATA,                           /**< Use the path /datatx. */
    SIM7080_FS_PATH_CUSTOMER,                       /**< Use the path /customer. */
} SIM7080_FS_Path_t;

#endif /* SIM7020_FS_DEFS_H_ */