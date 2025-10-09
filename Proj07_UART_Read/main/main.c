#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "display.h"

#define BUF_SIZE (1024) // Tamanho do buffer interno do driver da UART.

void app_main(void)
{
    // --- 1. CONFIGURAÇÃO DA UART ---
    uart_config_t uart_config = {
        .source_clk = UART_SCLK_DEFAULT,
        .baud_rate  = 9600,                     // Velocidade: 9600 bps
        .data_bits  = UART_DATA_8_BITS,         // 8 bits de dados
        .parity     = UART_PARITY_DISABLE,      // Sem paridade
        .stop_bits  = UART_STOP_BITS_1,         // 1 stop bit (8N1)
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE, // Sem controle de fluxo
    };

    uart_driver_install(0, BUF_SIZE * 2, 0, 0, NULL, 0); // Instala o driver da UART0.
    uart_param_config(0, &uart_config);                 // Aplica as configurações.
    uart_set_pin(0, GPIO_NUM_43, GPIO_NUM_44, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // Define os pinos de TX/RX.

    // --- 2. CONFIGURAÇÃO DO DISPLAY ---
    // Mapeia os pinos GPIO para cada segmento do display.
    int segs[7] = {
        GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, 
        GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10
    };

    // Inicializa os pinos do display como saídas.
    display_init(segs);

    // --- 3. LOOP PRINCIPAL ---
    uint8_t data[2]; // Buffer para 1 caractere de entrada + terminador nulo.
    int len = 0;     // Armazena o número de bytes lidos.
    const char* msg = "Escreva um número de 0 a 9: \r\n"; // Mensagem de prompt.
    
    display_set(segs, 0); // Mostra '0' no display ao iniciar.
    
    for(;;){ // Loop infinito
        
        // Envia a mensagem de prompt pela serial.
        uart_write_bytes(0, msg, strlen(msg));

        // Ler a mensagem do serial e salva o tamanho em len
        len = uart_read_bytes(0, data, 1, 3000/portTICK_PERIOD_MS);

        // Se algum dado foi recebido...
        if (len) {
            data[len] = '\0'; // Transforma em string C válida.
            printf("Recebido: %s\n", (char *) data); // Ecoa a entrada no terminal.
            
            // Valida se o caractere recebido é um dígito.
            if (data[0] >= '0' && data[0] <= '9') {
                // Converte o caractere ASCII para um número inteiro (ex: '5' -> 5).
                int numero = data[0] - '0';
                // Atualiza o display com o novo número.
                display_set(segs, numero);
            }
        }
        
        // Pausa a tarefa por 2 segundos.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}