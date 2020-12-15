#ifndef DATA_H
#define DATA_H

/****************************************************************************/
/*!                            Structs                                      */
typedef struct DeviceData
{
    int device_status;
    int temperature;
    int humidity;
} DeviceData;

/****************************************************************************/
/*!                         Functions                                       */

void parser(char * buffer);

#endif /* DATA_H_ */

