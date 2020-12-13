#ifndef SENSOR_H_
#define SENSOR_H_

/****************************************************************************/
/*!                         Macros                                          */
#define BUS "/dev/i2c-1" 

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief Function to initialize the sensor
 *
 * @return void
 *
 */
void initialize_sensor();

/*!
 *  @brief Thread function that get external temperature
 *
 * @param[in] args       : Pointer to access the data structure
 *
 * @return void
 *
 */
void *update_data_sensor(void *args);

#endif /* SENSOR_H_ */
