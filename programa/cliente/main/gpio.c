/****************************************************************************/
/*                       Header includes                                    */

#include "sdkconfig.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rtc_io.h"
#include "freertos/queue.h"

#include "dht11.h"
#include "mqtt.h"

/****************************************************************************/
/*! @file gpio.c
 * @brief Interface for led
 */
#include "gpio.h"

/****************************************************************************/
/*!                        Global Statements                                */

extern DeviceData device_data;
xQueueHandle intrQueue;

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to turn on or off. 
  */
void set_device_status(int status)
{
    gpio_set_level(DEVICE, status);
    device_data.device_status = status;
}

/**
  * @brief Function to update data sensor. 
  */
void update_device(DeviceData *device_data)
{
    int temperature_temp, humidity_temp;
    if (!DHT11_read().status)
    {
        temperature_temp = DHT11_read().temperature;
        humidity_temp = DHT11_read().humidity;

        if(temperature_temp > 0 && temperature_temp < 50)
            device_data->temperature = temperature_temp;
        
        if(humidity_temp > 20 && humidity_temp < 90)
            device_data->humidity = humidity_temp;
    }
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pin = (int) args;
    xQueueSendFromISR(intrQueue, &pin, NULL);
}

void processIntrButton(void *params)
{
    int pin;

    while (true)
    {
        if (xQueueReceive(intrQueue, &pin, portMAX_DELAY))
        {
            // De-bouncing
            int stat = gpio_get_level(pin);
            if (stat == 1)
            {
                gpio_isr_handler_remove(pin);
                while (gpio_get_level(pin) == stat)
                {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pin, gpio_isr_handler, (void *)pin);
                set_device_status(!device_data.device_status);
                sendDeviceStatus();
            }
        }
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
    gpio_pulldown_en(BUTTON);
    gpio_pullup_dis(BUTTON);
    gpio_set_intr_type(BUTTON, GPIO_INTR_POSEDGE);

    intrQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(processIntrButton, "Process Interruption Button", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON, gpio_isr_handler, (void *)BUTTON);
    DHT11_init(SENSOR);
}
