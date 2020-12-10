#ifndef TCP_H_
#define TCP_H_

/******************************************************************************/
/*                       Header includes                                      */

#include "data.h"

/****************************************************************************/
/*!                              Macros                                     */

#define CENTRAL_SERVER_IP        "192.168.0.53"
#define CENTRAL_SERVER_PORT      (unsigned short)10027
#define DISTRIBUTED_SERVER_IP    "192.168.0.52"
#define DISTRIBUTED_SERVER_PORT  (unsigned short)10127

/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief Function that init the tcp server
 *
 * @param[in] data       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void initialize_tcp_server(Data *data);

/*!
 *  @brief Thread function that receive the messages.
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *receive(void *args);

/*!
 *  @brief Thread function that send the messages.
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *submit(void *args);

#endif /* TCP_H_ */
