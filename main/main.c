/* ESP Test

   Demonstration of the ESP32 device that utilizes wifi, sleep mode, data storage, and LEDs.
   
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
#include "wifi_driver.h"
#include "nvs_driver.h"


#define GPIO_OUTPUT_LED    2
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_LED)
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */
#define TIME_TO_WAKE   120


static RTC_DATA_ATTR struct timeval sleep_enter_time;
static bool active_mode;
static void active_timer_callback(void* arg);
static const char* TAG = "example";

void app_main()
{
    /* Wakeup  */
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

    if(esp_sleep_get_wakeup_cause()==ESP_SLEEP_WAKEUP_TIMER){
        printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
    }

    //Create timer config struct
    const esp_timer_create_args_t active_timer_args = {
            .callback = &active_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .name = "active-timer"
    };
    esp_timer_handle_t active_timer;
    ESP_ERROR_CHECK(esp_timer_create(&active_timer_args, &active_timer));


    /* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_once(active_timer, TIME_TO_WAKE*uS_TO_S_FACTOR));
    ESP_LOGI(TAG, "Started timer, time since boot: %lld us", esp_timer_get_time());

    /* Start Wifi */
    wifi_startup();

    /* Configure LED */
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    /* Find number of blinks from the Nonvolatile Storage (NVS) */
    nvs_init();
    int led_blinks =nvs_read();
    ESP_LOGI(TAG,"stored_value: %i", nvs_read());

    /* Blink LED */
    for(int i = 0; i<led_blinks;i++){
        gpio_set_level(GPIO_OUTPUT_LED, 1);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(GPIO_OUTPUT_LED, 0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    /* Turn on LED */
    gpio_set_level(GPIO_OUTPUT_LED, 1);

    /*Loop until timer goes off */
    active_mode = true;
    while(active_mode){

        printf("Awake and hanging out\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    }

    /* Turn off LED */
    gpio_set_level(GPIO_OUTPUT_LED, 0);

    /* Shutdown  */
    rtc_gpio_isolate(GPIO_NUM_12); //turn off GPIO12 to minimize current consumption

    printf("Enabling timer wakeup, %ds\n", TIME_TO_SLEEP);
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    
    for (int i = 10; i >= 0; i--) {
        printf("Sleeping in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Sleeping now.\n");
    gettimeofday(&sleep_enter_time, NULL);
    esp_deep_sleep_start();

}

static void active_timer_callback(void* arg){
    active_mode = false;
}