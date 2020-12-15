#ifndef CONTROL_H_
#define CONTROL_H_

/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief Function to store log.
 *
 *  @param[in] message        :  char pointer to the message to be saved in the log
 * 
 *  @return void.
 *
 */
void store_data(char *message);

/*!
 *  @brief Function to handle program interuption.
 *
 *  @param[in] signal        :  interruption signal
 * 
 *  @return void.
 *
 */
void sig_handler(int signal);

/*!
 *  @brief Function to handle program interuption.
 *
 *  @param[in] signal        :  interruption signal
 * 
 *  @return void.
 *
 */
void alarm_handler(int signum);

/*!
 *  @brief Function to init the prog.
 *
 *  @return void.
 *
 */
void initialize_system();

#endif /* CONTROL_H_ */
