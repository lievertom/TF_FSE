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

#define NLAMP                   4
#define NOPEN_SENSOR            6
#define NPRESENCE_SENSOR        2

#define AIR_OFF                 0.0f

#define DELAY_ALARM             10

/****************************************************************************/
/*!                            Structs                                      */

typedef struct Data 
{
    bool alarm;
    unsigned char lamp;
    unsigned char air_turn;
    unsigned char open_sensor;
    unsigned char presence_sensor;
    float humidity;
    float temperature;
    float air_reference_temperature;
    int server_socket;
    pid_t alarm_pid;
} Data;

typedef struct Send
{
    unsigned char lamp;
    float air_reference_temperature;
} Send;

typedef struct Receive
{
    unsigned char air_turn;
    unsigned char open_sensor;
    unsigned char presence_sensor;
    float humidity;
    float temperature;
} Receive;

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

#endif /* DATA_H_ */
