#ifndef GPIO_H
#define GPIO_H

/****************************************************************************/
/*                       Header includes                                    */

#include "data.h"

/****************************************************************************/
/*!                              Macros                                     */
#define DEVICE    2
#define SENSOR    4
#define BUTTON    0
#define LOW       0
#define HIGH      1

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to turn on or off. 
  */
void set_device_status(int status);

/**
  * @brief Function to update data sensor. 
  */
void update_device(DeviceData * device_data);

/**
  * @brief Function to config gpio ports.
  */
void gpio_init();

#endif /* GPIO_H */
