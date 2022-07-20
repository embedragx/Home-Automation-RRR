/*
*   RRR Mini-Project Code  
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

/*********************
 *      MACROS
 *********************/
#define LED_SWITCH_1 32 //6th Pin from 39 side
#define LED_SWITCH_2 33 //5th Pin from 39 side
//#define LED_SWITCH_3 21 //11th Pin from 39 side
#define LED_SWITCH_3 12 //9th Pin from 39 side
#define LED_SWITCH_4 4  //5th Pin from 2 side
#define LED_SWITCH_5 2  //6th Pin from 2 side
#define LED_SWITCH_6 27 //7th Pin from 39 side


/**********************
 *  GLOBAL VARIABLES 
 **********************/
bool LED_SWITCH_1_STATE, LED_SWITCH_2_STATE, LED_SWITCH_3_STATE, LED_SWITCH_4_STATE, LED_SWITCH_5_STATE, LED_SWITCH_6_STATE, LED_SWITCH_7_STATE; // Stores States of 
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

static void switch_event_6(lv_obj_t * obj, lv_event_t event){
    switch(event) {
        case LV_EVENT_VALUE_CHANGED: 
            LED_SWITCH_6_STATE = lv_switch_get_state(obj);
            break;
    }
} 

//static void switch_event_7(lv_obj_t * obj, lv_event_t event){
//    switch(event) {
//        case LV_EVENT_VALUE_CHANGED: 
//            LED_SWITCH_7_STATE = lv_switch_get_state(obj);
//            break;
//    }
//}
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
    gpio_set_direction(LED_SWITCH_3, GPIO_MODE_OUTPUT);     // Pin 21 as output  
    
    gpio_reset_pin(LED_SWITCH_4);
    gpio_set_direction(LED_SWITCH_4, GPIO_MODE_OUTPUT);     // Pin 12 as output
    
    gpio_reset_pin(LED_SWITCH_5);
    gpio_set_direction(LED_SWITCH_5, GPIO_MODE_OUTPUT);     // Pin 4 as output
    
    gpio_reset_pin(LED_SWITCH_6);
    gpio_set_direction(LED_SWITCH_6, GPIO_MODE_OUTPUT);     // Pin 2 as output

    //gpio_reset_pin(LED_SWITCH_7);
    //gpio_set_direction(LED_SWITCH_7, GPIO_MODE_OUTPUT);     // Pin 27 as output

    while(1) {
        // Turning On/Off each output for Switches in GUI
        blink(LED_SWITCH_1_STATE, LED_SWITCH_1);
        blink(LED_SWITCH_2_STATE, LED_SWITCH_2);
        blink(LED_SWITCH_3_STATE, LED_SWITCH_3);
        blink(LED_SWITCH_4_STATE, LED_SWITCH_4);
        blink(LED_SWITCH_5_STATE, LED_SWITCH_5);
        blink(LED_SWITCH_6_STATE, LED_SWITCH_6);
        //blink(LED_SWITCH_7_STATE, LED_SWITCH_7);
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
    lv_scr_load(scr);                                          /*Load the screen*/

    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_NAVY);
    lv_obj_add_style(lv_scr_act(), LV_OBJ_PART_MAIN, &style_screen);  //turn the screen white

    lv_obj_t * cont= lv_cont_create(lv_scr_act(), NULL);

    
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_size(cont, 480, 39);

    lv_obj_t * mylabel = lv_label_create(cont, NULL);
    lv_obj_set_pos(mylabel, 0, 20);
    lv_obj_set_size(mylabel, 36, 39);
    lv_label_set_text(mylabel, LV_SYMBOL_WIFI);

    lv_obj_t * label = lv_label_create(cont, NULL);
    lv_obj_set_pos(label, 189, 20);
    lv_obj_set_size(label, 102, 19);
    lv_label_set_text(label, "RRR TECHNOLOGIES");

    /*Add labels to the buttons*/
    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);	/*Create a label on the first button*/
    lv_label_set_text(label1, "HUMIDITY");  
    lv_obj_set_pos(label1, 277, 128);
    lv_obj_set_size(label1, 161, 21);

    /*Add labels to the buttons*/
    lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);	/*Create a label on the first button*/
    lv_label_set_text(label2, "TIME");          	                /*Set the text of the label*/
    lv_obj_set_pos(label2, 28, 64);
    lv_obj_set_size(label2 , 186, 37);

    lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);	/*Create a label on the first button*/
    lv_label_set_text(label3, "TEMPERATURE");          	                /*Set the text of the label*/
    lv_obj_set_pos(label3, 280, 193);
    lv_obj_set_size(label3 , 155, 28);

    

    lv_obj_t *sw1 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw1, 14, 126);              	                /*hello Set the position of the button*/
    lv_obj_set_size(sw1 , 60, 35);

    lv_obj_t *sw2 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw2, 91, 126);              	                /*hello Set the position of the button*/
    lv_obj_set_size(sw2 , 60, 35);

    lv_obj_t *sw3 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw3, 168, 126);                               /*Set the position of the button*/
     lv_obj_set_size(sw3, 60, 35);

    lv_obj_t *sw4 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw4, 14, 190);
    lv_obj_set_size(sw4, 60, 35);
    
    lv_obj_t *sw5 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw5, 91, 190);
    lv_obj_set_size(sw5, 60, 35);
    
    lv_obj_t *sw6 = lv_switch_create(lv_scr_act(), NULL);
    lv_obj_set_pos(sw6, 168, 190);
    lv_obj_set_size(sw6, 60, 35);

    //lv_obj_t *sw7 = lv_switch_create(lv_scr_act(), NULL);
    //lv_obj_set_pos(sw7, 91, 254);
    //lv_obj_set_size(sw7, 60, 35);

    //lv_obj_t *sw8 = lv_switch_create(lv_scr_act(), NULL);
    //lv_obj_set_pos(sw8, 134, 254);
    //lv_obj_set_size(sw8, 60, 35);

    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_pos(slider, 263, 264);
    lv_obj_set_size(slider,190,14);
    lv_slider_set_range(slider, 0, 100);
    
     
    static lv_style_t style; 
    lv_style_init(&style);

    lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_letter_space(&style, LV_STATE_DEFAULT, 5);
    
    lv_obj_add_style(label2, LV_LABEL_PART_MAIN, &style);
    lv_obj_add_style(label3, LV_LABEL_PART_MAIN, &style);
    lv_obj_add_style(label1, LV_LABEL_PART_MAIN, &style);
    
    static lv_style_t style_switch; 
    lv_style_init(&style_switch);

    //lv_obj_set_style_local_bg_color()
    lv_obj_set_style_local_bg_color(slider ,LV_SLIDER_PART_INDIC, LV_EVENT_VALUE_CHANGED, LV_COLOR_GREEN);
    lv_obj_set_style_local_bg_color(sw1,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw1,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);
    //lv_obj_set_style_local_bg_color(sw1,LV_SWITCH_PART_KNOB, LV_STATE_CHECKED, LV_COLOR_BLUE);
    //lv_obj_set_style_local_bg_color(sw1,LV_SWITCH_PART_BG, LV_STATE_CHECKED, LV_COLOR_BLACK);

     lv_obj_set_style_local_bg_color(sw2,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw2,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

     lv_obj_set_style_local_bg_color(sw3,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw3,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

     lv_obj_set_style_local_bg_color(sw4,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw4,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

     lv_obj_set_style_local_bg_color(sw5,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw5,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

     lv_obj_set_style_local_bg_color(sw6,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_style_local_bg_color(sw6,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

    //lv_obj_set_style_local_bg_color(sw7,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    //lv_obj_set_style_local_bg_color(sw7,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);

    //lv_obj_set_style_local_bg_color(sw8,LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
    //lv_obj_set_style_local_bg_color(sw8,LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, LV_COLOR_GREEN);
    

    
    lv_obj_set_event_cb(sw1, switch_event_1);
    lv_obj_set_event_cb(sw2, switch_event_2);
    lv_obj_set_event_cb(sw3, switch_event_3);
    lv_obj_set_event_cb(sw4, switch_event_4);
    lv_obj_set_event_cb(sw5, switch_event_5);
    lv_obj_set_event_cb(sw6, switch_event_6);
    //lv_obj_set_event_cb(sw7, switch_event_7);
    //lv_obj_set_event_cb(sw8, switch_event_8`);
    
}

static void lv_tick_task(void *arg) {
    (void) arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
