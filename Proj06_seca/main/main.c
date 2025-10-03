#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <driver/pulse_cnt.h>


#define BUTTON_PLUS   GPIO_NUM_5
int count = 0;
int last_count = 0;
int show = 0;


void app_main(void) {

  gpio_set_direction(BUTTON_PLUS, GPIO_MODE_INPUT);         
  gpio_set_pull_mode(BUTTON_PLUS, GPIO_PULLUP_ENABLE);
  gpio_set_intr_type(BUTTON_PLUS, GPIO_INTR_LOW_LEVEL);

  pcnt_unit_config_t unit_config = {
      .high_limit = 10, 
      .low_limit = -10,
  };
  pcnt_unit_handle_t pcnt_unit = NULL;
  pcnt_new_unit(&unit_config, &pcnt_unit);

  pcnt_chan_config_t channel_config = {
      .edge_gpio_num = BUTTON_PLUS,
      .level_gpio_num = -1,
  };
  pcnt_channel_handle_t pcnt_channel = NULL;
  pcnt_new_channel(pcnt_unit, &channel_config, &pcnt_channel);

  pcnt_channel_set_edge_action(pcnt_channel,  PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE);

  pcnt_unit_enable(pcnt_unit);
  pcnt_unit_clear_count(pcnt_unit);
  pcnt_unit_start(pcnt_unit);

  
  max7219_spi_init();
  max7219_init_chip();
  draw_digit(show);

  while (1) {
      pcnt_unit_get_count(pcnt_unit, &count);

      if (count != last_count) {
          // antirruído simples: considera apenas transições +1
          if (count > last_count) {
              show = (show + 1) % 10;
              draw_digit(show);
              printf("pulso: %d  -> dígito: %d\n", count, show);
          }
          last_count = count;
          pcnt_unit_clear_count(pcnt_unit);
          last_count = 0;
      }

      vTaskDelay(pdMS_TO_TICKS(100)); // polling leve


  }
}
