/******************************************************************************/
/*                       Header includes                                      */

#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "control.h"
#include "window.h"
#include "data.h"

/******************************************************************************/
/*! @file mqtt.c
 * @brief Interface for mqtt
 */
#include "mqtt.h"

/****************************************************************************/
/*!                              Macros                                     */

#define REGEX_NEW_DEVICE "^{\"new_device\":\"\\w\\+\"}$"

/****************************************************************************/
/*!                         global statement                                */

MQTTClient client;

char * mac_address = NULL;

/****************************************************************************/
/*!                         Functions                                       */

void publish(char* topic, char* pay_load) 
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = pay_load;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char * payload = message->payload;

    if(!strcmp(topicName, MQTT_BASE_TOPIC))
    {
        if (parser(payload))
        {
            mac_address = payload;
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void subscribe(char * room)
{
    char topic[60];
    
    sprintf(topic, "%s/%s/temperatura",MQTT_BASE_ROOM_TOPIC,room);
    MQTTClient_subscribe(client, topic, 2);
    
    sprintf(topic, "%s/%s/umidade",MQTT_BASE_ROOM_TOPIC,room);
    MQTTClient_subscribe(client, topic, 2);
    
    sprintf(topic, "%s/%s/estado",MQTT_BASE_ROOM_TOPIC,room);
    MQTTClient_subscribe(client, topic, 2);
}


void initialize_mqtt()
{
    int rc;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, MQTT_ADDRESS, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    rc = MQTTClient_connect(client, &conn_opts);

    if (rc != MQTTCLIENT_SUCCESS)
    {
        sig_handler(SIGINT);
    }

    MQTTClient_subscribe(client, MQTT_BASE_TOPIC, 2);
}
