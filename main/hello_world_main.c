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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "soc/rtc.h"
#include "driver/rtc_io.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3        /* Time ESP32 will go to sleep (in seconds) */



static RTC_DATA_ATTR struct timeval sleep_enter_time;

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

    /* Blink LED */

    /* Turn on LED */

    /* Start Wifi */

    /* Loop and wait for GET Requests on '/configure_blink' */
    //look for the 'num' url request header




    /* Turn off LED */


    /* Shutdown  */
    rtc_gpio_isolate(GPIO_NUM_12); //turn off GPIO12 to minimize current consumption

    const int wakeup_time_sec = 20;
    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);
    

    for (int i = 10; i >= 0; i--) {
        printf("Sleeping in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Sleeping now.\n");

    esp_deep_sleep_start();

}

