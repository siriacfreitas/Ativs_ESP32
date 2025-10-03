# Projeto 01: Contador com Display de 7 Segmentos

## Questão Proposta

**Disciplina:** EGM0029 - Sistemas Embarcados para Controle e Automação  
**Período:** 2025.2  
**Projeto:** 01

> 1- Elabore um código fonte para implementar um contador incrementado por pushbutton e exibir a contagem entre 0 e 9 em um display de sete segmentos.

## Descrição do Projeto

Este projeto implementa um contador digital cíclico (0 a 9) em um microcontrolador ESP32. A contagem é incrementada a cada vez que um *pushbutton* é pressionado. Para garantir que cada toque no botão seja registrado de forma precisa e sem falhas, o sistema utiliza uma **interrupção de hardware (ISR)**, que é a maneira mais eficiente de tratar eventos externos.

O valor atual da contagem é exibido em um display de 7 segmentos do tipo **Ânodo Comum**, controlado diretamente por 7 pinos GPIO do ESP32. A lógica para acender os segmentos corretos para cada número está implementada através de uma matriz de consulta (*lookup table*) no código.

## Funcionalidades

  - Contador cíclico que vai de 0 a 9 e retorna a 0.
  - Incremento do contador acionado por um *pushbutton*.
  - Uso de interrupção de hardware para detecção imediata e confiável do toque no botão.
  - Exibição do dígito em um display de 7 segmentos.
  - Código modularizado com funções específicas para o controle do display.

## Hardware Utilizado


  - 1x Placa de Desenvolvimento ESP32
  - 1x Display de 7 Segmentos (modelo Ânodo Comum)
  - 1x Pushbutton (botão de pulso)
  - 7x Resistores de \~220Ω (para proteger os segmentos do display)
  - Jumpers para as conexões
  - 1x Protoboard

## Esquema de Conexões
<div>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/b29ed7ee-ad37-4379-9e93-1637aa2c799c" />
</div>
As conexões entre o ESP32 e os componentes são as seguintes:

| Display (Segmento) | Pino no ESP32 |
| :----------------- | :------------ |
| `a`                | `GPIO 4`      |
| `b`                | `GPIO 5`      |
| `c`                | `GPIO 6`      |
| `d`                | `GPIO 7`      |
| `e`                | `GPIO 8`      |
| `f`                | `GPIO 9`      |
| `g`                | `GPIO 10`     |
| **Pino Comum** | **`3V3`** |

| Componente | Pino no ESP32 | Descrição                                  |
| :--------- | :------------ | :----------------------------------------- |
| Pushbutton | `GPIO 1`      | Entrada do sinal (a outra perna vai no `GND`) |

## Estrutura do Código

  - `main/main.c`: Contém a lógica principal (`app_main`), a configuração do *pushbutton*, a inicialização do serviço de interrupção e o loop infinito que atualiza o display.
  - `main/display.c`: Implementa as funções declaradas em `display.h`. Contém a definição da matriz de consulta (`matriz`) e as funções para inicializar os pinos (`display_init`) e para exibir um número (`display_set`).
  - `main/display.h`: Arquivo de cabeçalho que "anuncia" a existência da matriz `matriz` (usando `extern`) e os protótipos das funções do display, permitindo que o `main.c` as utilize.
  - `CMakeLists.txt`: Arquivo de configuração do build na raiz do projeto.
  - `main/CMakeLists.txt`: Arquivo que informa ao sistema quais arquivos-fonte (`.c`) devem ser compilados para formar o componente principal.

