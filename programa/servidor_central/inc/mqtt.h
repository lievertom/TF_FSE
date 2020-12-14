#ifndef MQTT_H_
#define MQTT_H_

/******************************************************************************/
/*                       Header includes                                      */
#include <MQTTClient.h>

/****************************************************************************/
/*!                              Macros                                     */

#define MQTT_ADDRESS "tcp://test.mosquitto.org"
#define CLIENT_ID "SERVER"
#define MQTT_BASE_TOPIC "fse2020/170039251/dispositivos"
#define MQTT_BASE_ROOM_TOPIC "fse2020/170039251/"

/****************************************************************************/
/*!                            Structs                                      */


/****************************************************************************/
/*!                         Functions                                       */

void initialize_mqtt();

void publish(char* topic, char* pay_load);

void subscribe(char * room);

#endif /* MQTT_H_ */
