/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1
#define LED_SWITCH_1 32
#define LED_SWITCH_2 33
#define LED_SWITCH_3 12
#define LED_SWITCH_4 4
#define LED_SWITCH_5 2
#define SLIDER_SWITCH 27

/**********************
 *  GLOBAL VARIABLES 
 **********************/
bool LED_SWITCH_1_STATE, LED_SWITCH_2_STATE, LED_SWITCH_3_STATE, LED_SWITCH_4_STATE, LED_SWITCH_5_STATE; // Stores States of 
                                                                                                         // each UX switch

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void basic_layout(void);

// Call Back Function for each Switch
static void switch_event_1(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_1_STATE = lv_switch_get_state(obj);
            break;
    }
}

static void switch_event_2(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_2_STATE = lv_switch_get_state(obj);
            break;
    }
}

static void switch_event_3(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_3_STATE = lv_switch_get_state(obj);
            break;
    }
}

static void switch_event_4(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_4_STATE = lv_switch_get_state(obj);
            break;
    }
}

static void switch_event_5(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_5_STATE = lv_switch_get_state(obj);
            break;
    }
} 
//******************************************************************//

/**********************
 * FUNCTION PROTOTYPES
 **********************/
// ON/OFF Code to reduce redundancy
void blink(bool x, int16_t y){              // Bool 'x' is an Global variable containing state of each Switch 
                                            // and 'y' respective pin numbers
    if(x == false){
        gpio_set_level(y, 0);           
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    else{
        gpio_set_level(y, 1);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**********************
 * APPLICATION MAIN
 **********************/
void app_main() {

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096*2, NULL, 0, NULL, 1);   
    //Defining Pin Direction & Push/Pull up resitor mode
    gpio_reset_pin(LED_SWITCH_1);
    gpio_set_direction(LED_SWITCH_1, GPIO_MODE_OUTPUT);     // Pin 32 as output 

    gpio_reset_pin(LED_SWITCH_2);
    gpio_set_direction(LED_SWITCH_2, GPIO_MODE_OUTPUT);     // Pin 33 as output 
    
    gpio_reset_pin(LED_SWITCH_3);
    gpio_set_direction(LED_SWITCH_3, GPIO_MODE_OUTPUT);     // Pin 12 as output  
    
    gpio_reset_pin(LED_SWITCH_4);
    gpio_set_direction(LED_SWITCH_4, GPIO_MODE_OUTPUT);     // Pin 2 as output
    
    gpio_reset_pin(LED_SWITCH_5);
    gpio_set_direction(LED_SWITCH_5, GPIO_MODE_OUTPUT);     // Pin 3 as output
    
    while(1) {
        // Turning On/Off each output for Switches in GUI
        blink(LED_SWITCH_1_STATE, LED_SWITCH_1);
        blink(LED_SWITCH_2_STATE, LED_SWITCH_2);
        blink(LED_SWITCH_3_STATE, LED_SWITCH_3);
        blink(LED_SWITCH_4_STATE, LED_SWITCH_4);
        blink(LED_SWITCH_5_STATE, LED_SWITCH_5);
        //*********************************************************//
    }
}


/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {

    (void) pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);
	static lv_color_t *buf2 = NULL;
    static lv_disp_buf_t disp_buf;
	uint32_t size_in_px = DISP_BUF_SIZE;

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;


    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Create the demo application */
    basic_layout();

    while (1) {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
       }
    }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}

static void basic_layout(void){

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);                                           /*Load the screen*/

    /*Add labels to the buttons*/
    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);	/*Create a label on the first button*/
    lv_label_set_text(label1, "Lights/\nSwitches");          	/*Set the text of the label*/
    lv_obj_set_pos(label1, 20, 55);

    /*Add labels to the buttons*/
    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);	/*Create a label on the first button*/
    lv_label_set_text(label2, "Fan");          	                /*Set the text of the label*/
    lv_obj_set_pos(label2, 20, 109);

    lv_obj_t *sw1 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw1, 112, 52);              	                /*hello Set the position of the button*/

    lv_obj_t *sw2 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw2, 205, 52);                               /*Set the position of the button*/

    lv_obj_t *sw3 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw3, 298, 52);
    
    lv_obj_t *sw4 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw4, 391, 52);
    
    lv_obj_t *sw5 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw5, 112, 106);
    
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_pos(slider, 251, 114);
    lv_obj_set_size(slider,196,14);
    lv_slider_set_range(slider, 0, 100);
    
    lv_obj_set_event_cb(sw1, switch_event_1);
    lv_obj_set_event_cb(sw2, switch_event_2);
    lv_obj_set_event_cb(sw3, switch_event_3);
    lv_obj_set_event_cb(sw4, switch_event_4);
    lv_obj_set_event_cb(sw5, switch_event_5);
    
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
