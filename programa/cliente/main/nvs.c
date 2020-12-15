#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

extern char room[10];
extern int id;

void nvs_get_data()
{
    nvs_handle std_partision_handle;

    
    esp_err_t nvs_res = nvs_open("storage", NVS_READONLY, &std_partision_handle);
    
    if(nvs_res == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace: storage, not found");
    }
    else
    {
        size_t len;
        ESP_ERROR_CHECK(nvs_get_str(std_partision_handle, "room", room, &len));
        ESP_ERROR_CHECK(nvs_get_i32(std_partision_handle, "id", &id));
        nvs_close(std_partision_handle);
    }
}

void nvs_set_data()
{
    nvs_handle std_partision_handle;

    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &std_partision_handle));

    ESP_ERROR_CHECK(nvs_set_str(std_partision_handle, "room", room));
    ESP_ERROR_CHECK(nvs_set_i32(std_partision_handle, "id", id));

    nvs_commit(std_partision_handle);
    nvs_close(std_partision_handle);
}

void nvs_init ()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}
