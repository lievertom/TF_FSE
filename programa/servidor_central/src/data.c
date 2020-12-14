/******************************************************************************/
/*                       Header includes                                      */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cJSON.h"
#include "regex.h"

/******************************************************************************/
/*! @file data.c
 * @brief Data structure to save the data
 */
#include "data.h"

/****************************************************************************/
/*!                              Macros                                     */

#define REGEX_NEW_DEVICE "^{\"new_device\":\"\\(\\w\\w:\\)\\{5\\}\\w\\w\"}$"

const char * REGEX_DEVICE_DATA[] =
{
    "^{\"temperature\":\\w\\+}$",
    "^{\"humidity\":\\w\\+}$",
    "^{\"status\":\\w\\+}$",
};

const char * DEVICE_DATA[] =
{
    "temperature",
    "humidity",
    "status"
};

/****************************************************************************/
/*!                         Functions                                       */


/*!
 * @brief This function parse JSON.
 */
void parser(char * buffer)
{
    cJSON * json = cJSON_Parse (buffer);
    regex_t regex;

    if(!regcomp(&regex, REGEX_NEW_DEVICE, 0))
    {
        if (!regexec(&regex, buffer, 0, NULL, 0))
        {
            sprintf(buffer, "%s",cJSON_GetObjectItemCaseSensitive(json, "new_device")->valuestring);
        }
    } 

    regfree(&regex);
    cJSON_Delete(json);
}


/*!
 * @brief This function parse JSON.
 */
int parser_device_data(char * buffer, int  pointer)
{
    cJSON * json = cJSON_Parse (buffer);
    regex_t regex;
    int value = -1;

    if(!regcomp(&regex, REGEX_DEVICE_DATA[pointer], 0))
    {
        if (!regexec(&regex, buffer, 0, NULL, 0))
        {
            value = cJSON_GetObjectItemCaseSensitive(json, DEVICE_DATA[pointer])->valueint;
        }
    }

    regfree(&regex);
    cJSON_Delete(json);
    
    return value;
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