/******************************************************************************/
/*                       Header includes                                      */

#include <stdlib.h>
#include <unistd.h>

/******************************************************************************/
/*! @file alarm.c
 * @brief Interface for tcp server
 */
#include "alarm.h"

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief Function to control the alarm.
 */
unsigned char alarm_control(SystemData *system_data)
{
    for (int i = 0; i < NOPEN_SENSOR; i++)
        if (system_data->open_sensor&1<<i)
            return 1;

    for (int i = 0; i < NPRESENCE_SENSOR; i++)
        if (system_data->presence_sensor&1<<i)
            return 1;

    return 0;
}

/*!
 * @brief Function to play the alarm.
 */
void *play_alarm(void *args)
{
    SystemData *system_data = (SystemData *) args;

    if(system_data->alarm && alarm_control(system_data) && !system_data->alarm_pid)
    {
        system_data->alarm_pid = fork();
        if (!system_data->alarm_pid)
        {
            char *arguments[] = {
                COMMAND,
                PATH_AUDIO,
                NULL
            };

            alarm(0);

            while (1)
            {
                pid_t child = fork();
                if (!child)
                    execvp(COMMAND, arguments);
                // else
                //     wait(child);                
            }
        }
    }

    return NULL;
}