/******************************************************************************/
/*                       Header includes                                      */
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "bme280.h"
#include "control.h"
#include "data.h"

/******************************************************************************/
/*! @file sensor.c
 * @brief Interface for sensor driver 
 */
#include "sensor.h"


/****************************************************************************/
/*!                         global statement                                */

struct identifier id;
struct bme280_dev device;

/****************************************************************************/
/*!                         Functions                                       */

/*!
 * @brief This function reading the sensor's registers through I2C bus.
 */
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr)
{
    write(id.file_descriptor, &reg_addr, 1);
    read(id.file_descriptor, data, len);
    return BME280_OK;
}

/*!
 * @brief This function provides the delay for required time (Microseconds) as per the input provided in some of the
 * APIs
 */
void user_delay_us(uint32_t period, void *intf_ptr)
{
    usleep(period);
}

/*!
 * @brief This function for writing the sensor's registers through I2C bus.
 */
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr)
{
    uint8_t *buf;
    buf = (uint8_t *)malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);
    if (write(id.file_descriptor, buf, len + 1) < (uint16_t)len)
    {
        return BME280_E_COMM_FAIL;
    }
    free(buf);
    return BME280_OK;
}

/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in normal mode.
 */
int8_t stream_sensor_data_normal_mode()
{
	int8_t result;
	uint8_t settings_sel;

	/* Recommended mode of operation: Indoor navigation */
	device.settings.osr_h = BME280_OVERSAMPLING_1X;
	device.settings.osr_p = BME280_OVERSAMPLING_16X;
	device.settings.osr_t = BME280_OVERSAMPLING_2X;
	device.settings.filter = BME280_FILTER_COEFF_16;
	device.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	result = bme280_set_sensor_settings(settings_sel, &device);
    if (result != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor settings (code %+d).", result);
        return result;
    }
	result = bme280_set_sensor_mode(BME280_NORMAL_MODE, &device);
    if (result != BME280_OK)
    {
        fprintf(stderr, "Failed to set sensor mode (code %+d).", result);
	    return result;
    }
    sleep(1);
	return result;
}

/*!
 * @brief This function starts the bme280 sensor.
 */
void initialize_sensor()
{
    /* Variable to define the result */
    int8_t result;

    if ((id.file_descriptor = open(BUS, O_RDWR)) < 0)
    {
        printf("Failed to open the i2c bus %s\n", BUS);
        exit(1);
    }

    id.device_address = BME280_I2C_ADDR_PRIM;

    if (ioctl(id.file_descriptor, I2C_SLAVE, id.device_address) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
    }

    /* Make sure to select BME280_I2C_ADDR_PRIM or BME280_I2C_ADDR_SEC as needed */

    device.intf = BME280_I2C_INTF;
    device.read = user_i2c_read;
    device.write = user_i2c_write;
    device.delay_us = user_delay_us;

    /* Update interface pointer with the structure that contains both device address and file descriptor */
    device.intf_ptr = &id;

    /* Initialize the bme280 */
    result = bme280_init(&device);
    if (result != BME280_OK)
    {
        printf("Failed to initialize the device (code %+d).\n", result);
        exit(1);
    }
    result = stream_sensor_data_normal_mode();
    if (result != BME280_OK)
    {
        printf("Failed to stream sensor data (code %+d).\n", result);
        exit(1);
    }
}

/*!
 * @brief This function update temperature and humidity.
 */
void *update_data_sensor(void *args)
{
    SystemData *system_data = (SystemData *) args;
    struct bme280_data comp_data;
    /* Delay while the sensor completes a measurement */
    user_delay_us(70, device.intf_ptr);
    bme280_get_sensor_data(BME280_ALL, &comp_data, &device);

    if (comp_data.temperature > 0.0f && comp_data.temperature < 50.0f)
    {
        system_data->temperature = comp_data.temperature;
    }

    if (comp_data.humidity > 0.0f && comp_data.humidity < 100.0f)
    {
        system_data->humidity = comp_data.humidity;
    }
    
    return NULL;
}