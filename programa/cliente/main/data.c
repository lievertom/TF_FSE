/****************************************************************************/
/*                       Header includes                                    */

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "cJSON.h"

#include "nvs.h"
#include "gpio.h"

/****************************************************************************/
/*! @file data.c
 * @brief Data structure to save the data
 */
#include "data.h"

/****************************************************************************/
/*!                        Global Statements                                */

extern char room[50];
extern DeviceData device_data;
extern xSemaphoreHandle mqttSemaphore;

/****************************************************************************/
/*!                         Functions                                       */


/*!
 * @brief This function parse JSON.
 */
void parser(char * buffer)
{
    cJSON * json = cJSON_Parse (buffer);

    if (buffer[2] == 'r' && buffer[3] == 'o' && buffer[4] == 'o' && buffer[5] == 'm')
    {
        if (!strlen(room))
        {
            sprintf(room, "%s",cJSON_GetObjectItemCaseSensitive(json, "room")->valuestring);
            nvs_set_room();
            xSemaphoreGive(mqttSemaphore);
        }
    } 
    else if (buffer[2] == 'd' && buffer[3] == 'e' && buffer[4] == 'v' && buffer[5] == 'i' && buffer[6] == 'c' && buffer[7] == 'e')
    {
        int device = cJSON_GetObjectItemCaseSensitive(json, "device")->valueint;
        if (device != device_data.device_status)
        {
            device_data.device_status = set_device_status(device);
        }
    }
    cJSON_Delete(json);
}
