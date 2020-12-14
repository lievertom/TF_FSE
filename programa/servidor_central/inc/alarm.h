#ifndef ALARM_H_
#define ALARM_H_

/****************************************************************************/
/*!                              Macros                                     */

#define COMMAND     "omxplayer"
#define PATH_AUDIO  "ast/example.mp3"

/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief function that control the alarm
 *
 * @param[in] data       : Pointer to access the data structure
 *
 *  @return void
 *
 */
unsigned char alarm_control(SystemData *system_data);

/*!
 *  @brief Thread function that play the alarm.
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *play_alarm(void *args);


#endif /* ALARM_H_ */
