/****************************************************************************/
/*                       Header includes                                    */

#include <string.h>

#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_client.h"

#include "data.h"

/****************************************************************************/
/*! @file http_client.c
 * @brief Interface for http client
 */
#include "http_client.h"

/****************************************************************************/
/*!                              Macros                                     */

#define TAG "HTTP"

/****************************************************************************/
/*!                        Global Statements                                */

extern Field field;

/****************************************************************************/
/*!                         Functions                                       */

/**
  * @brief Function used for http event handler.
  */
esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    static char *output_buffer;
    static int output_len = 0; 
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) 
            {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                else 
                {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client)+1);
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                        output_buffer[esp_http_client_get_content_length(evt->client)] = '\0';
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;                
            }
            else
            {
                if (output_buffer == NULL) {
                    output_buffer = (char *) malloc(evt->data_len+1);
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                    output_buffer[evt->data_len] = '\0';
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                parser(output_buffer);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

/**
  * @brief Function used for http request.
  */
void http_request(char *url){
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handle,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
    ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
    }
    esp_http_client_cleanup(client);
}
