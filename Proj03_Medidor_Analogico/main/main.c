// Inclusão das bibliotecas necessárias para o projeto.
#include <stdio.h>                  // Biblioteca padrão de entrada e saída do C.
#include <driver/gpio.h>            // Biblioteca para controle dos pinos de I/O (GPIO) do ESP32.
#include <esp_adc/adc_oneshot.h>    // Biblioteca para o Conversor Analógico-Digital (ADC) no modo "one-shot".
#include <freertos/FreeRTOS.h>      // Biblioteca base do sistema operacional de tempo real FreeRTOS.
#include <freertos/task.h>          // Biblioteca para gerenciamento de tarefas do FreeRTOS (ex: vTaskDelay).
#include "display.h"                // Arquivo de cabeçalho customizado, provavelmente com a matriz de dígitos e a função display_init.

// Handle (identificador) para a unidade ADC que será configurada.
adc_oneshot_unit_handle_t adc_handle;

// Variável estática para armazenar o valor bruto lido pelo ADC (faixa de 0 a 4095).
static int valor_lido;
// Variável estática para armazenar o dígito calculado (0 a 9) a ser exibido.
static int digito;

// Função principal da aplicação, executada ao iniciar o ESP32.
void app_main(void)
{
    // --- CONFIGURAÇÃO DO CONVERSOR ANALÓGICO-DIGITAL (ADC) ---

    // Estrutura para configurar a unidade ADC. Estamos usando a ADC_UNIT_2.
    adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_2};
    // Estrutura para configurar as características de um canal ADC.
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Define a resolução do ADC (padrão é 12 bits, 0-4095).
        .atten    = ADC_ATTEN_DB_11       // Define a atenuação para 11dB, permitindo ler a faixa completa de tensão (0-3.3V).
    };
    // Inicializa a unidade ADC com a configuração definida e obtém o handle.
    adc_oneshot_new_unit(&init_config, &adc_handle);
    // Configura um canal específico (ADC_CHANNEL_2) na unidade ADC inicializada.
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_2, &config);

    // --- CONFIGURAÇÃO DO DISPLAY DE 7 SEGMENTOS ---

    // Mapeia os pinos do ESP32 para cada segmento do display.
    int segs[7] = {
        GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6,
        GPIO_NUM_7, GPIO_NUM_8, GPIO_NUM_9, GPIO_NUM_10
    };

    // Inicializa os pinos do display como saídas.
    // Esta função (display_init) deve estar definida no arquivo "display.c".
    display_init(segs);

    // --- LOOP PRINCIPAL ---
    // O laço for(;;) cria um loop infinito que manterá o programa rodando continuamente.
    for (;;) {
        // Realiza uma leitura do canal ADC configurado e armazena o resultado em 'valor_lido'.
        adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &valor_lido);

        // Converte o valor lido (0-4095) para um único dígito (0-9).
        // Mapeia para 0..9: floor( leitura/4096 * 10 )
        digito = (valor_lido * 10) / 4096;

        // Loop para varrer os 7 segmentos do display.
        for (int i = 0; i < 7; i++) {
            // Define o nível lógico (LIGADO/DESLIGADO) de cada pino do segmento.
            // Acessa a tabela 'matriz' (de "display.h") para obter o padrão do 'digito' atual.
            // Ex: para digito=1, matriz[1] terá o padrão {0,1,1,0,0,0,0}.
            gpio_set_level(i + 4, matriz[digito][i]);
        }

        // Pausa a execução desta tarefa por 100 milissegundos.
        // Isso evita que o processador fique 100% ocupado e permite que outras tarefas rodem.
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}