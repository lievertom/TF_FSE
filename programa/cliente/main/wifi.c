/****************************************************************************/
/*                       Header includes                                    */

#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "freertos/task.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/****************************************************************************/
/*! @file wifi.c
 * @brief Interface for wifi
 */
#include "wifi.h"

/****************************************************************************/
/*!                              Macros                                     */

#define WIFI_SSID           CONFIG_ESP_WIFI_SSID
#define WIFI_PASS           CONFIG_ESP_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

#define TAG                 "Wifi"

/****************************************************************************/
/*!                        Global Statements                                */

static EventGroupHandle_t s_wifi_event_group;

static int s_retry_num = 0;

extern xSemaphoreHandle wifiSemaphore;

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function to wifi event handler.
  */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // if (!s_retry_num)
        //     xSemaphoreTake(wifiSemaphore, portMAX_DELAY);
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            wifi_stop();
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Endereço IP recebido:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        xSemaphoreGive(wifiSemaphore);
    }
}

/**
  * @brief Function to init wifi.
  */
void wifi_init(){

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_LOGI(TAG, "wifi_init_sta finished.");

    wifi_start();
}

/**
  * @brief Function to start wifi.
  */
void wifi_start()
{
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 WIFI_SSID, WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 WIFI_SSID, WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/**
  * @brief Function to stop wifi.
  */
void wifi_stop()
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);
}

/**
  * @brief Function to format mac address.
  */
void setMacAddress(char * mac)
{
    uint8_t base_mac[6];
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(base_mac));
    sprintf(
        mac,
        "%02x:%02x:%02x:%02x:%02x:%02x",
        base_mac[0],
        base_mac[1],
        base_mac[2],
        base_mac[3],
        base_mac[4],
        base_mac[5]
    );
}
