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

#include "mqtt.h"

#define TAG "MQTT"

extern char room[10];
extern xSemaphoreHandle mqttSemaphore;
esp_mqtt_client_handle_t client; 
esp_mqtt_client_config_t mqtt_config = {
    .uri = "mqtt://test.mosquitto.org",
};

void mqtt_subscribe ()
{
    char topic[50];
    uint8_t mac;
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(&mac));
    if (strlen(room))
        xSemaphoreGive(mqttSemaphore);
    else
    {
        char message[20];
        sprintf(message, "{\"new_device\":%d}", mac);
        mqtt_send_message("fse2020/170039251/dispositivos", message);
    }    
    sprintf(topic, "fse2020/170039251/dispositivos/%d", mac);
    int msg_id = esp_mqtt_client_subscribe(client, topic, 0);
}

void mqtt_receive_message (esp_mqtt_event_handle_t event)
{
    printf("%.*s\r\n", event->data_len, event->data);
    parser(event->data);
    
}

void mqtt_send_message (char * topic, char * message)
{
    int id_msg = esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
    ESP_LOGI(TAG, "Mesnagem enviada, ID: %d", id_msg);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            if(!strlen(room))
                mqtt_subscribe();
            else
                xSemaphoreGive(mqttSemaphore);
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
