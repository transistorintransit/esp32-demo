/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

static esp_err_t err;
void nvs_init()
{
    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        // Read
        printf("Reading blink count from NVS ... ");
        int32_t num_blinks = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "num_blinks", &num_blinks);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Blink count = %d\n", num_blinks);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }


        // Close
        nvs_close(my_handle);
    }
}

int nvs_read(){
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    int32_t num_blinks = 0; // value will default to 0, if not set yet in NVS

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        // Read
        printf("Reading blink count from NVS ... ");
        err = nvs_get_i32(my_handle, "num_blinks", &num_blinks);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Blink count = %d\n", num_blinks);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }


        // Close
        nvs_close(my_handle);
    }
    return num_blinks;
}
int nvs_write(int count){
    
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);

    // Write
    printf("Updating blink count in NVS ... ");
    err = nvs_set_i32(my_handle, "num_blinks", count);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    printf("Committing updates in NVS ... ");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    nvs_close(my_handle);
    return err;

}