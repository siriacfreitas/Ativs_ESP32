#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_adc/adc_oneshot.h>

#define Kc  2.075
#define Ki  1.06
#define passo_integrador 0.001
#define Gdac 77.2727 // 255/3,3 (dac de 8 bits)
#define Gadc 0.0008 // 3,3/4096 (f adc com 12 bits)
int e0=0,ei=0;
float integrador=0.0,referencia=0.5;

void dac_set(uint8_t ei);
void dac_R2R_init();

adc_oneshot_unit_handle_t adc_handle;

void IRAM_ATTR timer_isr(void *arg);

void app_main(void)
{  
    const esp_timer_create_args_t timer_args = {
    .callback = &timer_isr,
    .name = "Timer"
    };
    esp_timer_handle_t timer_handler;

    esp_timer_create(&timer_args, &timer_handler);
    esp_timer_start_periodic(timer_handler, 1000);


    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_1};

    adc_oneshot_chan_cfg_t config = {.bitwidth = ADC_BITWIDTH_DEFAULT, 
                                     .atten    = ADC_ATTEN_DB_12};
    adc_oneshot_new_unit(&init_config, &adc_handle);                                     
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_0, &config); //GPIO1

    dac_R2R_init();

    for(;;){

        vTaskDelay(20000 / portTICK_PERIOD_MS);

        if (referencia==1.0){
            referencia =0.5;
        }else {
            referencia=1.0;
        }
    }
}

void IRAM_ATTR timer_isr(void *arg)
{ 
    float erro,proporcional,sinal_controle;
    dac_set(ei);
    erro = referencia - (Gadc* e0);
    proporcional = Kc* erro;
    integrador = integrador + (Kc* Ki* erro)*passo_integrador; // devido a formula escolhida (kc(s+Ki))/s
    sinal_controle= proporcional + integrador;
    ei= Gdac* sinal_controle;
    adc_oneshot_read(adc_handle,ADC_CHANNEL_0,&e0);
}

void dac_R2R_init(){
    for(int i = 4; i < 12; i++){
        gpio_set_direction(i, GPIO_MODE_OUTPUT);  
    }
}

void dac_set(uint8_t ei){
    for(int i =0; i < 8; i++){
        gpio_set_level(i+4, (1<<(i) & ei)>>i); 
    }
}