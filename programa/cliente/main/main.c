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
#include "mqtt_client.h"

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

DeviceData device_data = {0};

char room[10] = "";
int id = -1;

extern esp_mqtt_client_handle_t client; 


/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief function main.
  */
void app_main(void)
{
    nvs_init();
    nvs_get_data();

    wifiSemaphore = xSemaphoreCreateBinary();
    mqttSemaphore = xSemaphoreCreateBinary();

    gpio_init();
    wifi_init();

    esp_sleep_enable_timer_wakeup(30 * SECOND);

    xTaskCreate(&sendSensorData, "Send Sensor Data", 4096, NULL, 1, NULL);

    if (xSemaphoreTake(wifiSemaphore, portMAX_DELAY))
    {
        mqtt_start();
    }
}
