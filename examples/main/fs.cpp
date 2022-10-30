 /*
 * fs.cpp
 * 
 *  Copyright (C) Daniel Kampert, 2022
 *	Website: www.kampis-elektroecke.de
 *  File info: SIM70XX File System example.
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

#include <sdkconfig.h>

#ifdef CONFIG_DEMO_USE_FS

#include <esp_log.h>

#include "example.h"

static const char* TAG          = "FS";

#if(CONFIG_SIMXX_DEV == 7080)
    #define SIMXX_WriteFile(Device, File, Data, Size)               SIM7080_FS_Write(Device, SIM7080_FS_PATH_CUSTOMER, File, Data, Size)
    #define SIMXX_GetFileSize(Device, File, Size)                   SIM7080_FS_GetFileSize(Device, SIM7080_FS_PATH_CUSTOMER, File, Size)
    #define SIMXX_ReadFile(Device, File, Data, Size)                SIM7080_FS_Read(Device, SIM7080_FS_PATH_CUSTOMER, File, Data, Size)
    #define SIMXX_RenameFile(Device, Old, New)                      SIM7080_FS_Rename(Device, SIM7080_FS_PATH_CUSTOMER, Old, New)
    #define SIMXX_DeleteFile(Device, File)                          SIM7080_FS_Delete(Device, SIM7080_FS_PATH_CUSTOMER, File)
#endif

void FileSystem_Run(DEVICE_TYPE& p_Device)
{
    char* p_Buffer;
    uint32_t Size;
    std::string Data;
    std::string File;
    std::string NewFile;
    SIM70XX_Error_t Error;

    File = "Test.txt";
    NewFile = "NewFile.txt";
    Data = "Hello, World!";

	ESP_LOGI(TAG, "Run File System example...");

    ESP_LOGI(TAG, "Write file '%s'...", File.c_str());
    Error = SIMXX_WriteFile(p_Device, File, Data.c_str(), Data.size());
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not write file! Error: 0x%X", Error);
        return;
    }

    ESP_LOGI(TAG, "Reading file size...");
    Error = SIMXX_GetFileSize(p_Device, File, &Size);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not read file size! Error: 0x%X", Error);
        return;
    }

    ESP_LOGI(TAG, "     Size: %u", Size);

    ESP_LOGI(TAG, "Read file '%s'...", File.c_str());
    p_Buffer = (char*)calloc((Size + 1), sizeof(char));
    if(p_Buffer != NULL)
    {
        Error = SIMXX_ReadFile(p_Device, File, p_Buffer, Size);
        if(Error != SIM70XX_ERR_OK)
        {
            ESP_LOGE(TAG, "Can not read file! Error: 0x%X", Error);
            return;
        }

        ESP_LOGI(TAG, "     %s", p_Buffer);
    }

    ESP_LOGI(TAG, "Rename file '%s' into '%s'...", File.c_str(), NewFile.c_str());
    Error = SIMXX_RenameFile(p_Device, File, NewFile);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not rename file! Error: 0x%X", Error);
        return;
    }

    ESP_LOGI(TAG, "Remove file '%s'...", NewFile.c_str());
    Error = SIMXX_DeleteFile(p_Device, NewFile);
    if(Error != SIM70XX_ERR_OK)
    {
        ESP_LOGE(TAG, "Can not remove file! Error: 0x%X", Error);
        return;
    }
}

#endif