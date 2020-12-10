/******************************************************************************/
/*                       Header includes                                      */

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "tcp.h"
#include "data.h"
#include "alarm.h"
#include "window.h"

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
pthread_t receive_thread;

Data data;

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
    pthread_create(&send_thread, NULL, submit, (void *) &data);
}

/*!
 * @brief Function to handle program interuption.
 */
void alarm_handler(int signum)
{   
    alarm(1);
    pthread_join(output_thread, NULL);
    if (!data.alarm)
        pthread_create(&alarm_thread, NULL, play_alarm, (void *) &data);        

    pthread_create(&output_thread, NULL, output_values, (void *) &data);        
}

/*!
 * @brief Function to handle program interuption.
 */
void sig_handler (int signal)
{
    alarm(0);
    pthread_cancel(receive_thread);
    pthread_join(output_thread, NULL);
    if (data.alarm_pid)
        kill(data.alarm_pid, SIGKILL);
    end_window();
    close(data.server_socket);
    printf("exit, log saved to dat/data.csv\n");
    exit(0);
}

/*!
 * @brief Function to init the prog.
 */
void initialize_system()
{
    data.lamp = 0;
    data.alarm = 0;
    data.air_turn = 0;
    data.alarm_pid = 0;
    data.open_sensor = 0;
    data.presence_sensor = 0;

    initialize_tcp_server(&data);
    initialize_window();
    
    if (pthread_create(&input_thread, NULL, input_values, (void *) &data))
    {
        printf("Fail to create input thread\n");
        exit(1);
    }

    if(pthread_create(&receive_thread, NULL, receive, (void *) &data))
    {
        printf("Fail to create receive_thread\n");
        exit(2);
    }

    alarm(1);

    pthread_join(input_thread, NULL);
}
