/****************************************************************************/
/*                       Header includes                                    */

#include <stdio.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp32/rom/uart.h"
#include "esp_http_client.h"
#include "freertos/semphr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"

#include "gpio.h"
#include "wifi.h"
#include "data.h"

/****************************************************************************/
/*!                              Macros                                     */

#define SECOND 1000000

/****************************************************************************/
/*!                        Global Statements                                */

xSemaphoreHandle wifiSemaphoreLED = NULL;
xSemaphoreHandle wifiSemaphore = NULL;

DeviceData device_data = {0, 0, 0};

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief function main.
  */
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifiSemaphore = xSemaphoreCreateBinary();
    wifi_init();
    gpio_init();

    esp_sleep_enable_timer_wakeup(30 * SECOND);

            
    while(true)
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
        wifi_start();

        esp_sleep_wakeup_cause_t cause =  esp_sleep_get_wakeup_cause();

        if (cause == ESP_SLEEP_WAKEUP_TIMER)
        {
            update_device(&device_data);
        }
        else
        {
            device_data.device_status = invert_device_status(device_data.device_status);
        }

        printf ("%d %d %d\n", device_data.device_status, device_data.temperature, device_data.humidity);
    }
}
