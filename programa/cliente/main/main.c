/****************************************************************************/
/*                       Header includes                                    */

#include <stdio.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "freertos/semphr.h"

#include "led.h"
#include "wifi.h"
#include "data.h"
#include "http_client.h"

/****************************************************************************/
/*!                              Macros                                     */

#define IP_TACK_KEY CONFIG_IP_TACK_KEY
#define OPEN_WEATHER_MAP_KEY CONFIG_OPEN_WEATHER_MAP_KEY

#define REQUEST_DELAY 5*60*1000

/****************************************************************************/
/*!                        Global Statements                                */

xSemaphoreHandle wifiSemaphoreLED = NULL;
xSemaphoreHandle wifiSemaphoreHTTP = NULL;

Field field;

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Task function to blink led.
  */
void BlinkLED(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(wifiSemaphoreLED, portMAX_DELAY))
        {
            led_blink();
            xSemaphoreGive(wifiSemaphoreLED);
        }
    }
}

/**
  * @brief Task function used for http request location.
  */
void HTTPRequestLocation(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(wifiSemaphoreHTTP, portMAX_DELAY))
        {
            ESP_LOGI("Main Task", "Realiza HTTP Request");
            led_blink();
            char url[133];
            sprintf(url,
                    "http://api.ipstack.com/check?access_key=%s&fields=latitude,longitude",
                    IP_TACK_KEY);
            http_request(url);
            xSemaphoreGive(wifiSemaphoreHTTP);
            vTaskDelay(REQUEST_DELAY / portTICK_PERIOD_MS);
        }
    }
}

/**
  * @brief Task function used for http request weather.
  */
void HTTPRequestWeather(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(wifiSemaphoreHTTP, portMAX_DELAY))
        {
            ESP_LOGI("Main Task", "Realiza HTTP Request");
            led_blink();
            char url[150];
            sprintf(url,
                    "http://api.openweathermap.org/data/2.5/weather?lat=%lf&lon=%lf&units=metric&appid=%s",
                    field.latitude,
                    field.longitude,
                    OPEN_WEATHER_MAP_KEY);
            http_request(url);
            print();
            xSemaphoreGive(wifiSemaphoreHTTP);
            vTaskDelay(REQUEST_DELAY / portTICK_PERIOD_MS);
        }
        
    }
}

/**
  * @brief function main.
  */
void app_main(void)
{
    wifiSemaphoreLED = xSemaphoreCreateBinary();
    xSemaphoreGive(wifiSemaphoreLED);

    led_init();
    xTaskCreate(&BlinkLED, "Blink", 4096, NULL, 1, NULL);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifiSemaphoreHTTP = xSemaphoreCreateBinary();
    wifi_init();

    xTaskCreate(&HTTPRequestLocation, "HTTP Request Location", 4096, NULL, 1, NULL);
    xTaskCreate(&HTTPRequestWeather, "HTTP Request Weather", 4096, NULL, 1, NULL);
}
