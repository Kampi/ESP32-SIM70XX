#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include "sim7020.h"

static SIM7020_Config_t _Config = SIM7020_DEFAULT_CONF_1NCE(UART_NUM_1, SIM_BAUD_460800, GPIO_NUM_13, GPIO_NUM_14);

static SIM7020_t _Device;

static const char* TAG = "NB-IoT";

/** @brief          Add a value to a multipart POST request.
 *  @param p_Body   Pointer to HTTP message body
 *  @param Name     Name of the value
 *  @param Value    Value
 */
static void HTTP_AddValue(std::string* p_Body, std::string Name, std::string Value)
{
    p_Body->append("--data\r\n");
    p_Body->append("Content-Disposition: form-data; name=" + Name + "\r\n");
    p_Body->append("\r\n");
    p_Body->append(Value);
    p_Body->append("\r\n");
}

static void HTTP_Transmit(void)
{
    std::string Body;
    std::string Header;
    std::string Payload;
    SIM7020_HTTP_Socket_t Socket;

    HTTP_AddValue(&Body, "field", "1");

    Payload = "Hello, World!";

    Body += "--data\r\n";
    Body += "Content-Disposition: form-data; name=File; filename=Text.txt\r\n";
    Body += "\r\n";
    Body += Payload;
    Body += "\r\n";
    Body += "--data--\r\n";

    SIM7020_HTTP_AddToHeader("Accept", "*/*", &Header);
    SIM7020_HTTP_AddToHeader("Connection", "Keep-Alive", &Header);
    SIM7020_HTTP_AddToHeader("User-Agent", "SIM7020", &Header);
    SIM7020_HTTP_Create(&_Device, "https://webhook.site", &Socket);
    SIM7020_HTTP_Connect(&_Device, &Socket);
    SIM7020_HTTP_POST(&_Device, &Socket, "/044baec6-adc5-4e4c-aedb-6414c20497ff", "multipart/form-data; boundary=data", Header, Body);
    SIM7020_HTTP_Disconnect(&_Device, &Socket);
    SIM7020_HTTP_Destroy(&_Device, &Socket);
}

void Task_StartTransmissionTask(void)
{
    bool Active = false;

    ESP_LOGI(TAG, "SIM7020 HTTP POST example");

    Active = SIM7020_isActive(&_Config);
    ESP_LOGI(TAG, "Module active: %u", Active);

    if(SIM7020_Init(&_Device, &_Config) == SIM70XX_ERR_OK)
    {
        SIM7020_Info_t Info;

        SIM7020_Info_GetDeviceInformation(&_Device, &Info);
        SIM7020_Info_Print(&Info);

        HTTP_Transmit();
    }

    while(true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}