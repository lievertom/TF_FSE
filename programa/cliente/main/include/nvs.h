#ifndef NVS_H_
#define NVS_H_

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to get data in flash memory.
  */
void nvs_get_data();

/**
  * @brief Function to save data in flash memory.
  */
void nvs_set_data();

/**
  * @brief Function to init nvs.
  */
void nvs_init();

#endif /* NVS_H_ */
