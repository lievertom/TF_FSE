#ifndef MQTT_H_
#define MQTT_H_

/******************************************************************************/
/*                       Header includes                                      */
#include <MQTTClient.h>

/****************************************************************************/
/*!                              Macros                                     */

#define MQTT_ADDRESS "tcp://test.mosquitto.org"
#define CLIENT_ID "SERVER"
#define MQTT_BASE_TOPIC "fse2020/170039251/devices"
#define MQTT_BASE_ROOM_TOPIC "fse2020/170039251"

/****************************************************************************/
/*!                            Structs                                      */


/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief Function to init mqtt
 *
 *  @return void
 *
 */
void initialize_mqtt();

/*!
 *  @brief Function used to send message in a topic. 
 *
 * @param[in] topic       : broken topic
 * @param[in] pay_load    : message
 *
 *  @return void
 *
 */
void publish(char* topic, char* pay_load);

/*!
 *  @brief Function used to subscribe in a topic room. 
 *
 * @param[in] room       : room name
 *
 *  @return void
 *
 */
void subscribe(char * room);

/*!
 *  @brief Function used to set device status. 
 *
 * @param[in] mac       : device mac address 
 * @param[in] status    : device status
 *
 *  @return device status
 *
 */
void push (char * mac, int status);

#endif /* MQTT_H_ */
