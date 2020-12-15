/******************************************************************************/
/*                       Header includes                                      */

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
// #include <bcm2835.h>
#include <sys/mman.h>

#include "data.h"
#include "control.h"

/******************************************************************************/
/*! @file gpio.c
 * @brief Interface for actuator in GPIO
 */
#include "gpio.h"

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief This function configure the gpio ports.
 */
void configuration()
{
    bcm2835_gpio_fsel(LAMP_1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(LAMP_2, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_gpio_fsel(OPEN_SENSOR_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(OPEN_SENSOR_2, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(OPEN_SENSOR_3, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(OPEN_SENSOR_4, BCM2835_GPIO_FSEL_INPT);

    bcm2835_gpio_fsel(PRESENCE_SENSOR_1, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(PRESENCE_SENSOR_2, BCM2835_GPIO_FSEL_INPT);
}

/*!
 * @brief This function initialize the actuators.
 */
void initialize_gpio()
{
    if (!bcm2835_init())
    {
        printf("Error in bcm2835_init");
        exit(1);
    }
    configuration();
}

/*!
 * @brief This function turn on or off.
 */
void turn_on_off(int device, int turn)
{
    bcm2835_gpio_write(device, turn);
}


void *sensor_control (void *args)
{
    SystemData *system_data = (SystemData *) args;
    SystemData current_data = {0};
    
    current_data.open_sensor |= bcm2835_gpio_lev(OPEN_SENSOR_1)<<0;
    current_data.open_sensor |= bcm2835_gpio_lev(OPEN_SENSOR_2)<<1;
    current_data.open_sensor |= bcm2835_gpio_lev(OPEN_SENSOR_3)<<2;
    current_data.open_sensor |= bcm2835_gpio_lev(OPEN_SENSOR_4)<<3;

    current_data.presence_sensor |= bcm2835_gpio_lev(PRESENCE_SENSOR_1)<<0;
    current_data.presence_sensor |= bcm2835_gpio_lev(PRESENCE_SENSOR_2)<<1;

    if (current_data.open_sensor != system_data->open_sensor ||
            current_data.presence_sensor != system_data->presence_sensor)
    {
        system_data->open_sensor = current_data.open_sensor;
        system_data->presence_sensor = current_data.presence_sensor;
    }
    return NULL;
}