#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "cJSON.h"

#include "data.h"
#include "gpio.h"
#include "wifi.h"

#include "mqtt.h"

#define TAG "MQTT"

extern int id;
extern char room[10];
extern DeviceData device_data;
extern xSemaphoreHandle mqttSemaphore;
esp_mqtt_client_handle_t client; 
esp_mqtt_client_config_t mqtt_config = {
    .uri = "mqtt://test.mosquitto.org",
};
int mqtt_semaphore = 0;

void sendSensorData(void *params)
{
    char message[50];
    char topic[100];
    if (xSemaphoreTake(mqttSemaphore, portMAX_DELAY))
    {
        mqtt_semaphore = 1;
        while (true)
        {
            update_device(&device_data);
            sprintf(topic, "fse2020/170039251/%s/temperature", room);
            sprintf(message, "{\"id\":%d, \"temperature\":%d}", id, device_data.temperature);
            mqtt_send_message(topic, message);
            sprintf(topic, "fse2020/170039251/%s/humidity", room);
            sprintf(message, "{\"id\":%d, \"humidity\":%d}", id, device_data.humidity);
            mqtt_send_message(topic, message);
            vTaskDelay(30000 / portTICK_PERIOD_MS);
        }
    }
}

void sendDeviceStatus(void)
{
    if (mqtt_semaphore)
    {
        char message[50];
        char topic[100];
        sprintf(topic, "fse2020/170039251/%s/status", room);
        sprintf(message, "{\"id\":%d, \"status\":%d}", id, device_data.device_status);
        mqtt_send_message(topic, message);
    }
}

void mqtt_subscribe ()
{
    char topic[50];
    char mac[18];
    setMacAddress(mac);
    if (id > 0)
        xSemaphoreGive(mqttSemaphore);
    else
    {
        char message[70];
        sprintf(message, "{\"new_device\":\"%s\"}", mac);
        mqtt_send_message("fse2020/170039251/devices", message);
    }    
    sprintf(topic, "fse2020/170039251/devices/%s", mac);
    esp_mqtt_client_subscribe(client, topic, 2);
}

void mqtt_receive_message (esp_mqtt_event_handle_t event)
{
    printf("%.*s\r\n", event->data_len, event->data);
    parser(event->data);
}

void mqtt_send_message (char * topic, char * message)
{
    int id_msg = esp_mqtt_client_publish(client, topic, message, 0, 2, 0);
    printf("%s:%s\n", topic, message);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            mqtt_subscribe();
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            mqtt_receive_message(event);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
