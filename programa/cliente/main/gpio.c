/****************************************************************************/
/*                       Header includes                                    */

#include "sdkconfig.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"

#include "dht11.h"

/****************************************************************************/
/*! @file gpio.c
 * @brief Interface for led
 */
#include "gpio.h"

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to turn on or off. 
  */
int set_device_status(int status)
{
    gpio_set_level(DEVICE, status);
    return status;
}

/**
  * @brief Function to update data sensor. 
  */
void update_device(DeviceData * device_data)
{
    if (!DHT11_read().status)
    {
        device_data->temperature = DHT11_read().temperature;
        device_data->humidity = DHT11_read().humidity;
    }
}

/**
  * @brief Function to config gpio ports.
  */
void gpio_init()
{
    gpio_pad_select_gpio(DEVICE);
    gpio_set_direction(DEVICE, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(BUTTON);
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_wakeup_enable(BUTTON, GPIO_INTR_LOW_LEVEL); 

    esp_sleep_enable_gpio_wakeup();

    DHT11_init(SENSOR);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    // gpio_pullup_dis(gpio_num);
    // gpio_pulldown_en(gpio_num);

}
