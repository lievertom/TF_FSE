#ifndef GPIO_H_
#define GPIO_H_

/****************************************************************************/
/*!                              Macros                                     */
 
// #define LAMP_1             RPI_V2_GPIO_P1_11 
// #define LAMP_2             RPI_V2_GPIO_P1_12
 
// #define OPEN_SENSOR_1      RPI_V2_GPIO_P1_29 
// #define OPEN_SENSOR_2      RPI_V2_GPIO_P1_31 
// #define OPEN_SENSOR_3      RPI_V2_GPIO_P1_32 
// #define OPEN_SENSOR_4      RPI_V2_GPIO_P1_36 

// #define PRESENCE_SENSOR_1  RPI_V2_GPIO_P1_22 
// #define PRESENCE_SENSOR_2  RPI_V2_GPIO_P1_37

#define ON                 1
#define OFF                0

/****************************************************************************/
/*!                             Functions                                   */

/*!
 * @brief Function to initialize the actuators
 *
 * @return void
 *
 */
void initialize_gpio();

/*!
 *  @brief Thread function that control the presence and open sensor 
 *
 * @param[in] args       : Pointer to access the data structure
 *
 * @return void
 *
 */
void *sensor_control (void *args);

/*!
 *  @brief Thread function that control the lamps
 *
 * @param[in] args       : Pointer to access the data structure
 *
 * @return void
 *
 */
void *lamp_control (void *args);


/*!
 *  @brief Thread function that controls the AC
 *
 * @param[in] args       : Pointer to access the data structure
 *
 * @return void
 *
 */
void *ac_control (void *args);

void turn_on_off(int device, int turn);

#endif /* GPIO_H_ */