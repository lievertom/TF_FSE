/******************************************************************************/
/*                       Header includes                                      */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

/******************************************************************************/
/*! @file tcp.c
 * @brief Interface for tcp server
 */
#include "tcp.h"

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief This function receive the message.
 */
void *receive(void *args)
{
    Data *data = (Data *) args;
    Receive data_receive;
    int client_socket;
    struct sockaddr_in client_address;
    unsigned int client_Length = sizeof(client_address);

    while(1)
    {
        if((client_socket = accept(data->server_socket, (struct sockaddr *) &client_address, &client_Length)) < 0)
        {
            printf("Error in client socket\n");
        }
        else
        {
            recv(client_socket, &data_receive, sizeof(data_receive), 0);
            
            data->air_turn = data_receive.air_turn;
            data->humidity = data_receive.humidity;
            data->temperature = data_receive.temperature;
            data->open_sensor = data_receive.open_sensor;
            data->presence_sensor = data_receive.presence_sensor;

            close(client_socket);
        }
    }    
}

/*!
 * @brief This function send the message.
 */
void *submit(void *args)
{
    struct sockaddr_in server_address;
    int client_socket;

    if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Error in client socket\n");
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(DISTRIBUTED_SERVER_IP);
    server_address.sin_port = htons(DISTRIBUTED_SERVER_PORT);

    while(connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0);

    Data *data = (Data *) args;
    Send data_send;

    data_send.lamp = data->lamp;
    data_send.air_reference_temperature = data->air_reference_temperature;

    send(client_socket, &data_send, sizeof(data_send), 0);

    close(client_socket);

    return NULL;
}

/*!
 * @brief Function to init the tcp server.
 */
void initialize_tcp_server(Data *data)
{
    struct sockaddr_in server_address;

    if ((data->server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Error in server socket\n");
        exit(1);
    }

    int enable = 1;
    if (setsockopt(data->server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    {
        printf("Error in setsockopt(SO_REUSEADDR)");
    }

#ifdef SO_REUSEPORT
    if (setsockopt(data->server_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)) < 0) 
        printf("Error in setsockopt(SO_REUSEPORT)");
#endif

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(CENTRAL_SERVER_PORT);
 
    if(bind(data->server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    {
        printf("Error in bind\n");
        printf("Error code: %d\n", errno);
        exit(2);
    }

	if(listen(data->server_socket, 10) < 0)
    {
		printf("Falha no Listen\n");
        exit(3);
    }
}
