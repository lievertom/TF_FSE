/****************************************************************************/
/*                       Header includes                                    */

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/****************************************************************************/
/*! @file led.c
 * @brief Interface for led
 */
#include "led.h"

/****************************************************************************/
/*!                              Macros                                     */
#define LED 2

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to blink led.
  */
void led_blink()
{
    gpio_set_level(LED, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(LED, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

/**
  * @brief Function to config gpio ports.
  */
void led_init()
{
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}