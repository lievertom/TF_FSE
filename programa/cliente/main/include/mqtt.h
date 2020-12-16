#ifndef MQTT_H
#define MQTT_H

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to init mqtt.
  */
void mqtt_start();

/*!
 *  @brief Function used to send message
 *
 * @param[in] topic      : broker topic
 * @param[in] message    : message
 *
 *  @return void
 *
 */
void mqtt_send_message(char * topic, char * message);

/*!
 *  @brief Task Function used to update sersor data
 */
void sendSensorData(void *params);

/*!
 *  @brief Function used to update device status
 */
void sendDeviceStatus(void);

#endif
