// --- Inclusão de Bibliotecas ---
#include <freertos/FreeRTOS.h>   // Biblioteca do sistema operacional de tempo real
#include <freertos/task.h>      // Funções de gerenciamento de tarefas (vTaskDelay)
#include <driver/gpio.h>        // Driver para controle dos pinos GPIO
#include <esp_timer.h>          // Driver para o timer de alta resolução do ESP32
#include "display.h"            // Nosso driver para o display de 7 segmentos

// --- Variáveis Globais ---
// Flag para comunicação entre a interrupção do timer (ISR) e o loop principal.
// 'volatile' informa ao compilador que a variável pode mudar a qualquer momento.
volatile BaseType_t f_led = 0;

// Contador principal do programa (0-9).
volatile int count = 0;

// --- Protótipo da Função de Interrupção ---
// Declara a função que será chamada pelo timer.
// 'IRAM_ATTR' garante que o código da função seja colocado na memória RAM interna,
// o que é recomendado para rotinas de interrupção.
void IRAM_ATTR timer_isr(void *arg);


// --- Função Principal ---
void app_main(void)
{
    // --- Configuração do Timer de Alta Resolução ---
    // Define os argumentos para a criação do timer, especificando a função de callback.
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_isr,
        .name = "Timer"
    };
    esp_timer_handle_t timer_handler; // Handle (identificador) para o timer

    // Cria a instância do timer com os argumentos definidos.
    esp_timer_create(&timer_args, &timer_handler);
    // Inicia o timer para disparar periodicamente a cada 1.000.000 microssegundos (1 segundo).
    esp_timer_start_periodic(timer_handler, 1000000); 

    // --- Loop Principal ---
    for(;;){ // Loop infinito
      // Verifica se a flag foi acionada pela interrupção do timer.
      if (f_led){
        f_led = 0; // Zera a flag para "consumir" o evento do timer.
        count++;   // Incrementa o contador.
        // Se o contador passar de 9, ele retorna para 0.
        if (count >= 10){
          count = 0;
        }
      }

      // --- Atualização Direta do Display de 7 Segmentos ---
      // Itera sobre os 7 segmentos do display.
      for (int i=0; i<7; i++) {
        // Define o nível de cada pino (GPIO 4 a 10) de acordo com o padrão
        // para o dígito 'count' na matriz de consulta.
        gpio_set_level(i + 4, matriz[count][i]); 
      }
      
      // Pausa a tarefa por 100ms para não sobrecarregar a CPU.
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// --- Rotina de Serviço de Interrupção (ISR) do Timer ---
// Esta função é executada automaticamente pelo hardware a cada 1 segundo.
void IRAM_ATTR timer_isr(void *arg)
{
    // Define o nível do pino 5 (pode ser usado para depuração, como um "heartbeat").
    gpio_set_level(GPIO_NUM_5, f_led);
    // Aciona a flag, sinalizando para o loop principal que 1 segundo se passou.
    f_led = 1;
}