/****************************************************************************/
/*                       Header includes                                    */

#include <stdio.h>

#include "cJSON.h"

/****************************************************************************/
/*! @file data.c
 * @brief Data structure to save the data
 */
#include "data.h"

/****************************************************************************/
/*!                        Global Statements                                */

extern Field field;

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief This function print weather data.
 */
void print()
{
    printf(" Temperatura Atual: %.2lf °C\n", field.temperature);
    printf("Temperatura Mínima: %.2lf °C\n", field.temp_min);
    printf("Temperatura Máxima: %.2lf °C\n", field.temp_max);
    printf("          Humidade: %.2lf %%\n", field.humidity);
}

/*!
 * @brief This function parse JSON.
 */
void parser(char * buffer)
{
    cJSON * json = cJSON_Parse (buffer);

    if (buffer[2] == 'l')
    {
        field.latitude = cJSON_GetObjectItemCaseSensitive(json, "latitude")->valuedouble;
        field.longitude = cJSON_GetObjectItemCaseSensitive(json, "longitude")->valuedouble;
    }
    else if (buffer[2] == 'c')
    {
        cJSON * data = cJSON_GetObjectItem(json, "main");
        field.temperature = cJSON_GetObjectItemCaseSensitive(data, "temp")->valuedouble;
        field.temp_min = cJSON_GetObjectItemCaseSensitive(data, "temp_min")->valuedouble;
        field.temp_max = cJSON_GetObjectItemCaseSensitive(data, "temp_max")->valuedouble;
        field.humidity = cJSON_GetObjectItemCaseSensitive(data, "humidity")->valuedouble;
    }

    cJSON_Delete(json);
}
