#ifndef DATA_H_
#define DATA_H_

/******************************************************************************/
/*                       Header includes                                      */

#include <stdbool.h>

/****************************************************************************/
/*!                              Macros                                     */

#define PATH_DATA               "dat/data.csv"

#define DATE_SIZE               11
#define HOUR_SIZE               9

#define NLAMP                   2
#define NOPEN_SENSOR            4
#define NPRESENCE_SENSOR        2

#define MAX_DEVICE              5

#define DELAY_ALARM             10

/****************************************************************************/
/*!                            Structs                                      */

typedef struct SystemData 
{
    bool alarm;
    unsigned char lamp;
    unsigned char open_sensor;
    unsigned char presence_sensor;
    float humidity;
    float temperature;
    pid_t alarm_pid;
} SystemData;

typedef struct DeviceData
{
    int status;
    int humidity;
    int temperature;
    char * room;
    char mac[18];
} DeviceData;

/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief Thread function that saves data
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *save_data (void *args);

void parser(char * buffer);

int parser_device_data(char * buffer, int  pointer);

#endif /* DATA_H_ */
