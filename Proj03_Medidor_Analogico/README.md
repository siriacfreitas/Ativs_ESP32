# Projeto 03: Leitor Analógico com Display de 7 Segmentos

## Questão Proposta

**Disciplina:** EGM0029 - Sistemas Embarcados para Controle e Automação  
**Período:** 2025.2  
**Projeto:** 01

> 3- Elabore um código fonte para implementar um medidor analógico e exibir o seus valor dividido
 entre 0 e 9 em um display de sete segmentos.

## Descrição do Projeto

Este projeto implementa um voltímetro digital simplificado que lê uma tensão analógica de entrada, converte-a para um valor digital e exibe um dígito correspondente (0-9) em tempo real. A leitura é realizada pelo **Conversor Analógico-Digital (ADC)** de 12 bits do ESP32, que oferece uma resolução de 4096 níveis.

A lógica principal, contida em um loop infinito, realiza três tarefas sequenciais:

1.  Lê o valor bruto do ADC.
2.  Aplica uma fórmula de mapeamento matemático para converter a faixa de 0-4095 para a faixa de 0-9.
3.  Atualiza os pinos GPIO para acionar os segmentos corretos do display e mostrar o dígito.

Para garantir que o loop não consuma 100% do tempo de CPU, uma pequena pausa de 100ms (`vTaskDelay`) é inserida a cada ciclo, uma prática recomendada em sistemas que utilizam o FreeRTOS. O valor é exibido em um display de 7 segmentos do tipo **Ânodo Comum**.

## Funcionalidades

  - Leitura contínua de um sinal analógico através de um pino ADC.
  - Mapeamento matemático do valor lido (0-4095) para um dígito (0-9).
  - Exibição visual do dígito em um display de 7 segmentos.
  - Atualização do display a cada 100 milissegundos.
  - Utiliza as APIs do ESP-IDF para configuração do ADC e controle dos GPIOs.

## Hardware Utilizado

  - 1x Placa de Desenvolvimento ESP32
  - 1x Display de 7 Segmentos (modelo **Ânodo Comum**)
  - 1x Potenciômetro de 10kΩ
  - 7x Resistores de \~220Ω (para proteger os segmentos do display)
  - Jumpers para as conexões
  - 1x Protoboard

## Esquema de Conexões

<div>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/d535ea1a-2c0f-42dd-96ca-c5d80b999d35" />
<div>

As conexões são divididas entre o display e o potenciômetro:

| Componente         | Pino no ESP32  |
| :----------------- | :------------- |
| Display `a`        | `GPIO 4`       |
| Display `b`        | `GPIO 5`       |
| Display `c`        | `GPIO 6`       |
| Display `d`        | `GPIO 7`       |
| Display `e`        | `GPIO 8`       |
| Display `f`        | `GPIO 9`       |
| Display `g`        | `GPIO 10`      |
| **Display Comum** | **`3V3`** |
| Potenciômetro Saída| `GPIO 27` (ADC2\_CH2) |
| Potenciômetro VCC  | `3V3`          |
| Potenciômetro GND  | `GND`          |

## Explicação do Código

A lógica do programa é construída sobre dois pilares principais: a **configuração e leitura do ADC** e o **mapeamento do valor para exibição no display**.

#### 1\. Configuração do Conversor Analógico-Digital (ADC)

Para ler um sinal analógico, o ADC do ESP32 precisa ser inicializado. Utilizamos a API `adc_oneshot` para leituras sob demanda.

```c
// Configura a unidade ADC a ser usada (ADC_UNIT_2)
adc_oneshot_unit_init_cfg_t init_config = {.unit_id = ADC_UNIT_2};
adc_oneshot_new_unit(&init_config, &adc_handle);

// Configura o canal ADC
adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT, // Resolução de 12 bits (0-4095)
    .atten    = ADC_ATTEN_DB_11       // Atenuação para ler até 3.3V
};
adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_2, &config);
```

  - Primeiro, inicializamos a unidade ADC (`ADC_UNIT_2`).
  - Em seguida, configuramos um canal (`ADC_CHANNEL_2`, correspondente ao GPIO 27), definindo sua resolução (`bitwidth`) e atenuação (`atten`). A atenuação de 11dB é crucial para permitir que o ADC meça a faixa completa de tensão de 0 a 3.3V.

#### 2\. Loop Principal: Leitura, Mapeamento e Exibição

O `for(;;)` contém toda a lógica de operação contínua do sistema.

```c
for(;;){
    // 1. Leitura
    adc_oneshot_read(adc_handle, ADC_CHANNEL_2, &valor_lido);

    // 2. Mapeamento
    digito = (valor_lido * 10) / 4096;

    // 3. Exibição
    for (int i=0; i<7; i++) {
        // Para Ânodo Comum, a lógica é invertida (0=LIGA, 1=DESLIGA)
        // A matriz deve conter 0s para os segmentos a serem acesos.
        gpio_set_level(i+4, matriz[digito][i]); 
    }

    // 4. Pausa
    vTaskDelay(100/portTICK_PERIOD_MS);
}
```

  - **Leitura:** A função `adc_oneshot_read` é chamada para obter o valor digital (0-4095) e armazená-lo em `valor_lido`.
  - **Mapeamento:** A linha `digito = (valor_lido * 10) / 4096;` realiza a conversão. Ela escala a faixa de 0-4095 para uma faixa de 0-9.99..., e a conversão para inteiro trunca a parte decimal, resultando em um dígito de 0 a 9.
  - **Exibição:** O laço `for` itera sobre os 7 segmentos. Ele utiliza a `matriz` de consulta para determinar o estado de cada pino (LIGADO/DESLIGADO) para formar o dígito desejado. Como o display é Ânodo Comum, um nível lógico `0` acende o segmento e `1` o apaga.
  - **Pausa:** `vTaskDelay` cede o controle ao sistema operacional por 100ms, tornando o sistema eficiente e responsivo.
