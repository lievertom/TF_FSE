/****************************************************************************/
/*                       Header includes                                    */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp32/rom/uart.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"

#include "gpio.h"
#include "wifi.h"
#include "data.h"
#include "mqtt.h"
#include "nvs.h"

/****************************************************************************/
/*!                              Macros                                     */

#define SECOND 1000000

/****************************************************************************/
/*!                        Global Statements                                */

xSemaphoreHandle wifiSemaphore = NULL;
xSemaphoreHandle mqttSemaphore = NULL;

DeviceData device_data = {0, 0, 0};

char room[10] = "";

/****************************************************************************/
/*!                         Functions                                       */

void conectadoWifi(void *params)
{
    // while (true)
    {
        if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY))
        {
            mqtt_start();
            xSemaphoreGive(wifiSemaphore);
        }
    }
}

void trataComunicacaoComServidor(void *params)
{
    char message[50];
    char topic[100];
    if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY))
    {
        // while (true)
        {
            sprintf(message, "{'temperature':%d}", device_data.temperature);
            sprintf(topic, "fse2020/170039251/dispositivos/%s/temperatura", room);
            mqtt_send_message(topic, message);
            sprintf(topic, "fse2020/170039251/dispositivos/%s/umidade", room);
            sprintf(message, "{'humidity':%d}", device_data.humidity);
            mqtt_send_message(topic, message);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            xSemaphoreGive(wifiSemaphore);
        }
    }
}

/**
  * @brief function main.
  */
void app_main(void)
{
    nvs_init();
    nvs_get_room();

    wifiSemaphore = xSemaphoreCreateBinary();
    mqttSemaphore = xSemaphoreCreateBinary();

    gpio_init();
    wifi_init();

    esp_sleep_enable_timer_wakeup(30 * SECOND);

    // xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
    // xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);

    int a = 0;
    while (true)
    {
        if (!rtc_gpio_get_level(BUTTON))
        {
            do
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            } while (!rtc_gpio_get_level(BUTTON));
        }

        uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

        esp_light_sleep_start();
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        if (cause == ESP_SLEEP_WAKEUP_GPIO)
            device_data.device_status = set_device_status(!device_data.device_status);
        update_device(&device_data);
        wifi_start();
        if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY))
        {
            mqtt_start();
            xSemaphoreGive(wifiSemaphore);
        }
        trataComunicacaoComServidor(NULL);

        xSemaphoreTake(wifiSemaphore, portMAX_DELAY);
        // xSemaphoreTake(mqttSemaphore, portMAX_DELAY);
        printf("%d %d %d\n", device_data.device_status, device_data.temperature, device_data.humidity);
    
    }
}
