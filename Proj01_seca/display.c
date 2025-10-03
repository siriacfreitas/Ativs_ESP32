#include "display.h"
#include <driver/gpio.h>

void display_init(const int *segs){
    for(int i = 0; i < 7; i++){
        gpio_set_direction(segs[i], GPIO_MODE_OUTPUT);  
    }
}

void display_set(const int *segs, int count){
    for (int i = 0; i < 7; i++) {
        gpio_set_level(segs[i], matriz[count][i]); 
    }
}

