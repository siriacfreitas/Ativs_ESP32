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
<div align="center">
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

## Estrutura do Código

  - `main/main.c`: Contém a lógica principal da aplicação (`app_main`), a configuração do periférico PCNT para ler o botão e o loop que atualiza o display.
  - `main/max7219.c`: Contém as funções de baixo nível para controlar o display MAX7219, como a inicialização do SPI, o envio de dados e a configuração dos registradores.
  - `main/max7219.h`: Arquivo de cabeçalho com as definições de pinos, mapa de registradores, a "fonte" (bitmap) dos dígitos e os protótipos das funções do driver do display.
  - `main/CMakeLists.txt`: Arquivo de configuração do build que informa ao sistema quais arquivos-fonte (`.c`) devem ser compilados.
