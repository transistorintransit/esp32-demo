/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3        /* Time ESP32 will go to sleep (in seconds) */

static RTC_DATA_ATTR struct timeval sleep_enter_time;
static bool active_mode;
static void active_timer_callback(void* arg);
static const char* TAG = "example";

void app_main()
{
    /* Print chip information */
    #ifdef PRINT_INFO
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
    #endif


    /* Wakeup  */
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

    if(esp_sleep_get_wakeup_cause()==ESP_SLEEP_WAKEUP_TIMER){
        printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
    }

    //Create timer config stuff
    const esp_timer_create_args_t active_timer_args = {
            .callback = &active_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .name = "active-timer"
    };
    esp_timer_handle_t active_timer;
    ESP_ERROR_CHECK(esp_timer_create(&active_timer_args, &active_timer));

    const int active_time_sec = 120;

    /* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_once(active_timer, active_time_sec*uS_TO_S_FACTOR));
    ESP_LOGI(TAG, "Started timer, time since boot: %lld us", esp_timer_get_time());


    /* Blink LED */

    /* Turn on LED */

    /* Start Wifi */

    /* Loop and wait for GET Requests on '/configure_blink' */
    //look for the 'num' url request header

    active_mode = true;
    while(active_mode){

        printf("Awake and hanging out\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }



    /* Turn off LED */


    /* Shutdown  */
    rtc_gpio_isolate(GPIO_NUM_12); //turn off GPIO12 to minimize current consumption

    const int wakeup_time_sec = 60;
    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * uS_TO_S_FACTOR);
    

    for (int i = 10; i >= 0; i--) {
        printf("Sleeping in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Sleeping now.\n");

    esp_deep_sleep_start();

}

static void active_timer_callback(void* arg){
    active_mode = false;
}