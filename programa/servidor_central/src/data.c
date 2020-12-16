/******************************************************************************/
/*                       Header includes                                      */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "cJSON.h"
#include "regex.h"
#include "window.h"

/******************************************************************************/
/*! @file data.c
 * @brief Data structure to save the data
 */
#include "data.h"

/****************************************************************************/
/*!                         global statement                                */

extern DeviceData device_data[MAX_DEVICE];

/****************************************************************************/
/*!                         Functions                                       */


/*!
 * @brief This function parse JSON.
 */
bool parser(char * buffer)
{
    cJSON * json = cJSON_Parse (buffer);
    bool aux = false;

    cJSON * mac = cJSON_GetObjectItemCaseSensitive(json, "new_device");
    if (mac)
    {
        sprintf(buffer, "%s", mac->valuestring);
        aux = true;
    }

    cJSON_Delete(json);
    return aux;
}

/*!
 *  @brief Function used to get the device id
 */
int get_id (char * buffer)
{
    cJSON * json = cJSON_Parse (buffer);
    int id = -1;

    cJSON * id_json = cJSON_GetObjectItemCaseSensitive(json, "id");
    if (id_json)
    {
        id = id_json->valueint;
    }

    cJSON_Delete(json);
    return id;
}

/*!
 * @brief This function parse JSON.
 */
int parser_device_data(char * buffer, char * field)
{
    cJSON * json = cJSON_Parse (buffer);
    int value = -1;

    cJSON * value_json = cJSON_GetObjectItemCaseSensitive(json, field);
    if (value_json)
    {
        value = value_json->valueint;
    }

    cJSON_Delete(json);
    return value;
}

/*!
 *  @brief Function used to update device data
 */
void update_device_data(int id, char * field, int value)
{
    if (!strcmp(field,"temperature"))
        device_data[id].temperature = value;
    else if (!strcmp(field,"humidity"))
        device_data[id].humidity = value;
    else if (!strcmp(field,"status"))
        if(device_data[id].status != value)
            switch_device((unsigned char)(id+3), id);
}

/*!
 * @brief This function get date and hour of system.
 */
void date_hour (char * date, char * hour)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date, DATE_SIZE, "%F", timeinfo);
    strftime(hour, HOUR_SIZE, "%X", timeinfo);
}

/*!
 *  @brief Thread function that saves data
 */
void *save_data (void *args)
{
    char *message = (char *) args;
    char date[DATE_SIZE];
    char hour[HOUR_SIZE];

    date_hour (date, hour);
    FILE *file = fopen(PATH_DATA,"a+");

    /* If file is not defined, return */
    if(file == NULL)
    {
        printf("Error: File is not defined!\n");
        return NULL;
    }

    fseek (file, 0, SEEK_END);

    /* Add header if file is empty */
    if(ftell(file) == 0)
    {
        fprintf(file, "Date, Hour, Message\n");
    }

    /* Store data mean rounded with one decimal place */
    fprintf(file, "%s, %s, %s\n", date, hour, message);

    fclose(file);
    return NULL;
}