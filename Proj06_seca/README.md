# Projeto 06: Contador de Pulsos com Display MAX7219

Este repositório contém o código-fonte para o Projeto 06, que implementa um contador de pulsos utilizando um pushbutton e exibe o resultado em uma matriz de LED 8x8 controlada pelo driver MAX7219.

## Questão Proposta

**Disciplina:** EGM0029 - Sistemas Embarcados para Controle e Automação  
**Período:** 2025.2  
**Projeto:** 03
> "1- Elabore um código fonte para implementar a contagem de pulsos em uma entrada digital e exibir o valor da contagem em uma matriz LED 8×8, a qual será controlada através do CI MAX7219 (ver datasheet). A contagem será sempre crescente, de uma unidade e limitada entre 0 e 9 em formato de anel, ou seja, ao se incrementar o valor 9, o próximo valor será o 0. Para incrementar o valor da contagem será utilizado um pushbutton."
> 

## Visão Geral

O projeto utiliza o microcontrolador ESP32 para ler a entrada de um pushbutton através do periférico de hardware **Pulse Counter (PCNT)**, garantindo uma detecção de pulso eficiente e livre de *bouncing* (tremulação) por software. Cada vez que o botão é pressionado, a contagem é incrementada. O valor atual da contagem (de 0 a 9) é então exibido como um grande caractere numérico em um display de matriz de LED 8x8.

## Funcionalidades

  - Contagem cíclica de 0 a 9.
  - Incremento da contagem a cada pulso de um pushbutton.
  - Lógica de "anel" (wrap-around): após o 9, a contagem retorna para 0.
  - Exibição visual do dígito em uma matriz de LED 8x8.
  - Interface com o driver MAX7219 via comunicação SPI.
  - Uso do periférico PCNT do ESP32 para uma contagem de pulsos robusta.

## Hardware Necessário

  - 1x Placa de Desenvolvimento ESP32
  - 1x Módulo de Matriz LED 8x8 com MAX7219
  - 1x Pushbutton (Botão de pulso)
  - Jumpers para as conexões

## Conexões (Wiring)
<div>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/de4eec54-5fcf-4f7a-a4ae-359ed5927b22" />
</div>
         
As conexões entre o ESP32 e os componentes são as seguintes:

| Módulo MAX7219 | Pino no ESP32 | Descrição              |
| :------------- | :------------ | :--------------------- |
| `VCC`          | `5V` ou `3V3` | Alimentação            |
| `GND`          | `GND`         | Terra                  |
| `DIN`          | `GPIO 12`     | Dados SPI (MOSI)       |
| `CS`           | `GPIO 10`     | Chip Select (LOAD)     |
| `CLK`          | `GPIO 11`     | Clock SPI (SCLK)       |

| Componente | Pino no ESP32 | Descrição                                  |
| :--------- | :------------ | :----------------------------------------- |
| Pushbutton | `GPIO 5`      | Entrada do sinal (a outra perna vai no GND) |


## Explicação do Código

A arquitetura do software utiliza o periférico de hardware **Pulse Counter (PCNT)** do ESP32, uma abordagem robusta e eficiente para contar eventos externos sem sobrecarregar a CPU com interrupções constantes.

#### 1\. Configuração do Pulse Counter (PCNT)

Em vez de usar uma interrupção de GPIO padrão, o pino do botão é associado diretamente ao hardware do PCNT.

```c
// --- Configuração do canal do PCNT para monitorar o pino do botão ---
pcnt_chan_config_t channel_config = {
    .edge_gpio_num = BUTTON_PLUS, // Pino que gera o pulso
    .level_gpio_num = -1,
};
pcnt_new_channel(pcnt_unit, &channel_config, &pcnt_channel);

// --- Define a ação do contador ---
pcnt_channel_set_edge_action(pcnt_channel, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
```

  - A configuração do canal (`pcnt_chan_config_t`) vincula o pino físico (`BUTTON_PLUS`) ao contador.
  - A função `pcnt_channel_set_edge_action` é a chave: ela instrui o hardware a **incrementar o contador automaticamente** na borda de subida do sinal. Como o pino usa um resistor de *pull-up*, a borda de subida ocorre quando o botão é **solto**. Isso funciona como um excelente filtro de *debounce* em hardware.

#### 2\. O Loop Principal e a Lógica de Polling

O loop principal (`while(1)`) não precisa de *flags* ou interrupções complexas. Ele simplesmente "pergunta" ao periférico PCNT qual é a contagem atual.

```c
// Lê o valor atual do contador de hardware
pcnt_unit_get_count(pcnt_unit, &count);

// Verifica se a contagem mudou (botão foi pressionado e solto)
if (count != last_count) {
    // Filtro: só age se a contagem aumentou
    if (count > last_count) {
        show = (show + 1) % 10; // Incrementa o dígito a ser exibido
        draw_digit(show);       // Atualiza o display
    }
    // Zera a contagem para a próxima detecção
    pcnt_unit_clear_count(pcnt_unit);
    last_count = 0;
}
```

  - A cada 100ms (definido pelo `vTaskDelay`), o código lê o contador de hardware com `pcnt_unit_get_count`.
  - Se o valor (`count`) for maior que o último valor lido (`last_count`), o programa entende que um pulso válido ocorreu.
  - A variável de exibição (`show`) é então incrementada, o display é atualizado com `draw_digit(show)`, e o contador de hardware é **zerado** com `pcnt_unit_clear_count`, deixando-o pronto para detectar o próximo pulso.

#### 3\. Controle do Display (MAX7219)

As funções `max7219_spi_init()` e `max7219_init_chip()` preparam a comunicação SPI e configuram o display. A função `draw_digit(show)` é a responsável por enviar, via SPI, o padrão de bits (bitmap) do número `show` para o display, que então o renderiza na matriz 8x8.

Esta abordagem com PCNT é muito eficiente, pois delega a tarefa de contar e filtrar os pulsos do botão para um hardware dedicado, liberando a CPU para outras tarefas (como, neste caso, atualizar o display).
