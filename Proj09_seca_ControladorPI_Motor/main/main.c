#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_timer.h>
#include <driver/gptimer.h>

#define ap  26.67 //4T =~150 ms
#define Kp  21.336 // Kp/ap = 0.8 
#define Kc  2.075
#define Ki  1.06
#define passo_integrador 0.001
#define Gdac 1 //77.2727 // 255/3,3 (dac de 8 bits)
#define Gadc 1 //0.0129    // 3,3/255 
int e0=0,ei=0;
float integrador=0.0,referencia=80;
int cont_pwm=0;
uint64_t count =0;
gptimer_handle_t gptimer = NULL;

void IRAM_ATTR timer_isr(void *arg);
void IRAM_ATTR pulso_isr(void *arg);

void dac_R2R_init();
void dac_set(uint8_t ei);

void app_main(void)
{
        unsigned char cnt_led=0;

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = 4000, 
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .channel          = LEDC_CHANNEL_0,
        .timer_sel        = LEDC_TIMER_0,
        .intr_type        = LEDC_INTR_DISABLE,
        .gpio_num         = GPIO_NUM_35, 
        .duty             = 0, 
        .hpoint           = 0
    };
    ledc_channel_config(&ledc_channel);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);   


    gpio_set_direction(GPIO_NUM_47, GPIO_MODE_INPUT);         
    gpio_set_pull_mode(GPIO_NUM_47, GPIO_PULLUP_ENABLE);
    gpio_set_intr_type(GPIO_NUM_47, GPIO_INTR_POSEDGE);


    gptimer_config_t timer_config ={
        .clk_src= GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000};

    gptimer_new_timer(&timer_config,&gptimer);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);


    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_47,pulso_isr,NULL);

        const esp_timer_create_args_t timer_args = {
        .callback = &timer_isr,
        .name = "Timer"
    };
    esp_timer_handle_t timer_handler;

    esp_timer_create(&timer_args, &timer_handler);
    esp_timer_start_periodic(timer_handler, 1000);

    dac_R2R_init();

    for(;;){
        vTaskDelay(250 / portTICK_PERIOD_MS);

        if (referencia==180){
            referencia =80;
        }else {
            referencia=180;
        }
        cont_pwm++;
        if (cont_pwm>=255){
            cont_pwm=0;
        }else if (cont_pwm<=0) {
            cont_pwm=255;
        }
    }
}

void IRAM_ATTR pulso_isr(void *arg)
{
    gptimer_get_raw_count(gptimer,&count);
    count = count/25; 
    e0 = 255-count;
    //dac_set(255-count);
    //printf("count: %llu\n",count);
    gptimer_set_raw_count(gptimer,0);

}

void IRAM_ATTR timer_isr(void *arg)
{ 
    float erro,proporcional,sinal_controle;
    if (ei>255){
        ei=255;
    }else if (ei<0) {
        ei=0;
    }
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,ei);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0); 
    erro = referencia - (Gadc* e0);
    proporcional = Kc* erro;
    integrador = integrador + (Kc* Ki* erro)*passo_integrador; // devido a formula escolhida (kc(s+Ki))/s
    sinal_controle= proporcional + integrador;
    ei= Gdac* sinal_controle;
    if (e0>255){
        e0=255;
    }else if (e0<0) {
        e0=0;
    }
    dac_set(e0);
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