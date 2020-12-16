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

/*!
 *  @brief Function used to get the device mac address
 *
 * @param[in] buffer       : JSON message
 *
 *  @return return true if get the new device mac address
 *
 */
bool parser(char * buffer);

/*!
 *  @brief Function used to get the field value
 *
 * @param[in] buffer       : JSON message
 * @param[in] field        : device field
 *
 *  @return return field value
 *
 */
int parser_device_data(char * buffer, char * field);

/*!
 *  @brief Function used to get the device id
 *
 * @param[in] buffer       : JSON message
 *
 *  @return return device id
 *
 */
int get_id (char * buffer);

/*!
 *  @brief Function used to update device data
 *
 * @param[in] id       : device id
 * @param[in] field    : device field
 * @param[in] value    : field value
 *
 *  @return void
 *
 */
void update_device_data(int id, char * field, int value);

#endif /* DATA_H_ */
