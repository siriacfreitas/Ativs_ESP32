#include <stdio.h>
#include <freertos/FreeRTOS.h>   // Biblioteca do sistema operacional de tempo real
#include "freertos/task.h"      // Funções de gerenciamento de tarefas (como vTaskDelay)
#include <driver/gpio.h>        // Driver para controle dos pinos de entrada/saída (GPIO)
#include "display.h"            // Inclui nosso driver para o display de 7 segmentos

// --- Variáveis Globais ---

// Flag para comunicação entre a interrupção (ISR) e o loop principal.
// 'volatile' avisa o compilador que esta variável pode mudar a qualquer momento.
volatile BaseType_t f_button = 0;

// Variável que armazena a contagem atual (0-9).
volatile int count = 0;

// --- Protótipo da Função de Interrupção ---

// Declara a função que será executada quando a interrupção do botão ocorrer.
// 'IRAM_ATTR' garante que o código da função seja colocado na memória RAM interna,
// o que é um requisito para rotinas de interrupção rápidas.
void button_isr();


// --- Função Principal ---
void app_main(void)
{
    // Mapeia os pinos do ESP32 para cada segmento do display.
    int segs[7] = {
        GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, 
        GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10
    };

    // Inicializa os pinos do display como saídas.
    display_init(segs);

    // --- Configuração do pino do botão ---
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);         // Define o pino do botão como entrada.
    gpio_set_pull_mode(GPIO_NUM_1, GPIO_PULLUP_ENABLE);      // Habilita o resistor de pull-up interno.
    gpio_set_intr_type(GPIO_NUM_1, GPIO_INTR_LOW_LEVEL);     // Define a interrupção para ser acionada em nível baixo.
    
    // --- Configuração do serviço de interrupção ---
    gpio_install_isr_service(0); // Instala o serviço de interrupção global para os GPIOs.
    // Associa nossa função 'button_isr' ao pino do botão (GPIO_NUM_1).
    gpio_isr_handler_add(GPIO_NUM_1, button_isr, NULL);

    // --- Loop Principal (Super Loop) ---
    for(;;){ // Loop infinito
        // Verifica se a flag foi acionada pela interrupção.
        if (f_button) {
            f_button = 0; // Zera a flag para "consumir" o evento do botão.
            count = (count + 1) % 10; // Incrementa o contador, com 'wrap-around' de 9 para 0.
        }

        // Atualiza o display com o valor atual do contador.
        display_set(segs, count);
        // Pausa a tarefa por 500ms para economizar CPU.
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

// --- Rotina de Serviço de Interrupção (ISR) ---
// Esta função é executada AUTOMATICAMENTE pelo hardware quando o botão é pressionado.
void IRAM_ATTR button_isr()
{
    // A única ação da ISR é sinalizar para o loop principal que o botão foi pressionado.
    // ISRs devem ser o mais curtas e rápidas possível.
    f_button = 1;
}