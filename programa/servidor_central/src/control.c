/******************************************************************************/
/*                       Header includes                                      */

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "data.h"
#include "alarm.h"
#include "window.h"
#include "gpio.h"

/******************************************************************************/
/*! @file control.c
 * @brief Interface for threads control
 */
#include "control.h"

/****************************************************************************/
/*!                        Global Statements                                */

pthread_t log_thread;
pthread_t send_thread;
pthread_t input_thread;
pthread_t alarm_thread;
pthread_t output_thread;

SystemData system_data = {0};

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief Function used to save log.
 */
void store_data (char *message)
{
    pthread_join(log_thread, NULL);
    pthread_create(&log_thread, NULL, save_data, (void *) message);
}

/*!
 * @brief Function used to send message.
 */
void push()
{
    pthread_join(send_thread, NULL);
}

/*!
 * @brief Function to handle program interuption.
 */
void alarm_handler(int signum)
{   
    alarm(1);
    pthread_join(output_thread, NULL);
    if (!system_data.alarm)
        pthread_create(&alarm_thread, NULL, play_alarm, (void *) &system_data);        

    pthread_create(&output_thread, NULL, output_values, (void *) &system_data);        
}

/*!
 * @brief Function to handle program interuption.
 */
void sig_handler (int signal)
{
    alarm(0);
    pthread_join(output_thread, NULL);
    if (system_data.alarm_pid)
        kill(system_data.alarm_pid, SIGKILL);
    end_window();
    // turn_on_off(LAMP_1, OFF);
    // turn_on_off(LAMP_2, OFF);
    // bcm2835_close();
    printf("exit, log saved to dat/data.csv\n");
    exit(0);
}

/*!
 * @brief Function to init the prog.
 */
void initialize_system()
{
    initialize_window ();
    
    if (pthread_create(&input_thread, NULL, input_values, (void *) &system_data))
    {
        printf("Fail to create input thread\n");
        exit(1);
    }

    alarm(1);

    pthread_join(input_thread, NULL);
}
