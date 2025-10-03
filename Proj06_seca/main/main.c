// --- Inclusão de bibliotecas ---

#include <stdio.h>
#include "max7219.h"           // Funções específicas do display MAX7219
#include "freertos/FreeRTOS.h" // Sistema operacional de tempo real
#include "freertos/task.h"     // Gerenciamento de tarefas
#include "driver/spi_master.h" // Comunicação SPI para o display
#include "driver/gpio.h"       // Controle dos pinos de entrada/saída
#include "esp_err.h"           // Tratamento de erros do ESP-IDF
#include <driver/pulse_cnt.h>  // Periférico de contador de pulsos (PCNT)


// --- Definições e Variáveis Globais ---
#define BUTTON_PLUS   GPIO_NUM_5 // Define o pino do botão
int count = 0;                 // Armazena a contagem de pulsos do hardware
int last_count = 0;            // Guarda a contagem anterior para detectar mudanças
int show = 0;                  // Dígito (0-9) a ser exibido no display

// --- Função Principal ---
void app_main(void) {

  // --- Configuração do pino do botão ---
  gpio_set_direction(BUTTON_PLUS, GPIO_MODE_INPUT);        // Define o pino como entrada
  gpio_set_pull_mode(BUTTON_PLUS, GPIO_PULLUP_ENABLE);     // Habilita o resistor de pull-up interno
  gpio_set_intr_type(BUTTON_PLUS, GPIO_INTR_LOW_LEVEL);    // (Opcional) Define tipo de interrupção, usado pelo PCNT

  // --- Configuração da unidade do Contador de Pulsos (PCNT) ---
  pcnt_unit_config_t unit_config = {
      .high_limit = 10,  // Limite superior da contagem
      .low_limit = -10, // Limite inferior da contagem
  };
  pcnt_unit_handle_t pcnt_unit = NULL;
  pcnt_new_unit(&unit_config, &pcnt_unit); // Cria a unidade de contagem

  // --- Configuração do canal do PCNT para monitorar o pino do botão ---
  pcnt_chan_config_t channel_config = {
      .edge_gpio_num = BUTTON_PLUS, // Pino que gera o pulso (borda)
      .level_gpio_num = -1,         // Pino de controle de nível (não usado)
  };
  pcnt_channel_handle_t pcnt_channel = NULL;
  pcnt_new_channel(pcnt_unit, &channel_config, &pcnt_channel); // Associa o canal à unidade

  // --- Define a ação do contador: Aumenta na borda de subida (botão solto) ---
  pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE);

  // --- Inicia o contador de pulsos ---
  pcnt_unit_enable(pcnt_unit); // Habilita a unidade
  pcnt_unit_clear_count(pcnt_unit); // Zera a contagem
  pcnt_unit_start(pcnt_unit);  // Começa a contar

  // --- Inicialização do display MAX7219 ---
  max7219_spi_init();  // Inicia a comunicação SPI
  max7219_init_chip(); // Configura os registradores do display
  draw_digit(show);    // Exibe o dígito inicial (0)

  // --- Loop Principal ---
  while (1) {
      // Lê o valor atual do contador de hardware
      pcnt_unit_get_count(pcnt_unit, &count);

      // Verifica se a contagem mudou (botão foi pressionado)
      if (count != last_count) {
          // Filtro antirruído: só age se a contagem aumentou
          if (count > last_count) {
              show = (show + 1) % 10; // Incrementa o dígito e volta a 0 após o 9
              draw_digit(show);         // Atualiza o display com o novo dígito
              printf("pulso: %d  -> dígito: %d\n", count, show);
          }
          // Zera a contagem para a próxima detecção
          last_count = count;
          pcnt_unit_clear_count(pcnt_unit);
          last_count = 0;
      }

      // Pequena pausa para não sobrecarregar a CPU
      vTaskDelay(pdMS_TO_TICKS(100));
  }
}