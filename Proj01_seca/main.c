#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>    
#include "display.h"

volatile BaseType_t f_button=0;

volatile int count=0;

void IRAM_ATTR button_isr();

void app_main(void)
{
    int segs[7] = {
        GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, 
        GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10
    };

    display_init(segs); // já é um ponteiro para o primeiro elemento

    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);         
    gpio_set_pull_mode(GPIO_NUM_1, GPIO_PULLUP_ENABLE);
    gpio_set_intr_type(GPIO_NUM_1, GPIO_INTR_LOW_LEVEL);
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_1, button_isr, NULL);

    for(;;){
        if (f_button) {
            f_button = 0;
            count = (count + 1) % 10;
        }

        display_set(segs, count);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void IRAM_ATTR button_isr()
{
    f_button=1;
}