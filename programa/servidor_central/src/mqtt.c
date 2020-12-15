/******************************************************************************/
/*                       Header includes                                      */

#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#include "control.h"
#include "window.h"
#include "regex.h"
#include "data.h"

/******************************************************************************/
/*! @file mqtt.c
 * @brief Interface for mqtt
 */
#include "mqtt.h"

/****************************************************************************/
/*!                              Macros                                     */

#define REGEX_TOPIC_TEMPERATURE "^fse2020/170039251/\\w\\+/temperature$"
#define REGEX_TOPIC_HUMIDITY "^fse2020/170039251/\\w\\+/humidity$"
#define REGEX_TOPIC_STATUS "^fse2020/170039251/\\w\\+/status$"

/****************************************************************************/
/*!                         global statement                                */

MQTTClient client;

char * mac_address = NULL;

/****************************************************************************/
/*!                         Functions                                       */

void push (char * mac, int status)
{
    char topic[100];
    char pay_load[60];
    sprintf(topic, "%s/%s", MQTT_BASE_TOPIC, mac);
    sprintf(pay_load, "{\"status\":%d}", status);
    publish(topic, pay_load);
}

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

void parser_topic(char * topic)
{
    regex_t regex;

    if(!regcomp(&regex, REGEX_TOPIC_TEMPERATURE, 0))
    {
        if (!regexec(&regex, topic, 0, NULL, 0))
        {
            sprintf(topic, "temperature");
            return;
        }
    }
    if(!regcomp(&regex, REGEX_TOPIC_HUMIDITY, 0))
    {
        if (!regexec(&regex, topic, 0, NULL, 0))
        {
            sprintf(topic, "humidity");
            return;
        }
    }
    if(!regcomp(&regex, REGEX_TOPIC_STATUS, 0))
    {
        if (!regexec(&regex, topic, 0, NULL, 0))
        {
            sprintf(topic, "status");
            return;
        }
    }
}

int on_message(void *context, char *topic, int topicLen, MQTTClient_message *message)
{
    char * payload = message->payload;

    if(!strcmp(topic, MQTT_BASE_TOPIC))
    {
        if (parser(payload))
        {
            mac_address = payload;
        }
    }
    else
    {
        int id, value;
        id = get_id(payload);
        if (id > -1)
        {
            parser_topic(topic);
            value = parser_device_data(payload, topic);
            if (value > -1)
                update_device_data(id, topic, value);
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic);
    return 1;
}

void subscribe(char * room)
{
    char topic[60];
    
    sprintf(topic, "%s/%s/temperature",MQTT_BASE_ROOM_TOPIC,room);
    MQTTClient_subscribe(client, topic, 2);
    
    sprintf(topic, "%s/%s/humidity",MQTT_BASE_ROOM_TOPIC,room);
    MQTTClient_subscribe(client, topic, 2);
    
    sprintf(topic, "%s/%s/status",MQTT_BASE_ROOM_TOPIC,room);
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
