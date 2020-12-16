#ifndef WIFI_H
#define WIFI_H

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to init wifi.
  */
void wifi_init();

/**
  * @brief Function to start wifi.
  */
void wifi_start();

/**
  * @brief Function to stop wifi.
  */
void wifi_stop();

/**
  * @brief Function to format mac address.
 * 
 * @param[in] mac       : device mac address
 * 
  */
void setMacAddress(char * mac);

#endif /* WIFI_H_ */
