# Projeto 02: Contador Automático com Display de 7 Segmentos Utilizando Timer

## Questão Proposta

**Disciplina:** EGM0029 - Sistemas Embarcados para Controle e Automação  
**Período:** 2025.2  
**Projeto:** 01

> 2- Elabore um código fonte para implementar um contador incrementado por um GPTimer e exibir a contagem entre 0 e 9 em um display de sete segmentos.

## Descrição do Projeto

Este projeto implementa um contador digital automático que cicla de 0 a 9, com o valor sendo incrementado a cada segundo. Para a temporização precisa, foi utilizado um timer de alta resolução do ESP32 (implementado com a API `esp_timer`), que cumpre o requisito de usar um timer de hardware para controlar a contagem.

O timer é configurado para gerar uma interrupção periódica a cada 1 segundo. A rotina de serviço de interrupção (ISR) é mantida extremamente rápida, apenas sinalizando para o loop principal através de uma *flag* `volatile`. O loop principal, ao detectar a *flag*, é responsável por incrementar a variável de contagem e atualizar o display de 7 segmentos. Este método garante que a lógica principal não bloqueie o sistema e que a temporização seja consistente.

O valor atual da contagem é exibido em um display de 7 segmentos do tipo Ânodo Comum, controlado diretamente pelos pinos GPIO do ESP32.

## Funcionalidades

  - Contador automático e cíclico que vai de 0 a 9.
  - Incremento da contagem a cada 1 segundo, sem necessidade de intervenção externa.
  - Utiliza o `esp_timer` do ESP-IDF para uma temporização precisa baseada em hardware.
  - Comunicação segura entre a interrupção e o loop principal através de uma *flag* `volatile`.
  - Exibição visual do dígito em um display de 7 segmentos.

## Hardware Utilizado

  - 1x Placa de Desenvolvimento ESP32
  - 1x Display de 7 Segmentos (modelo Ânodo Comum)
  - 7x Resistores de \~220Ω (para proteger os segmentos do display)
  - Jumpers para as conexões
  - 1x Protoboard

## Esquema de Conexões
<div>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/d4aa312b-9c71-4fda-b07e-2fa3ede7ae6e" />
<div>
As conexões entre o ESP32 e o display são as seguintes:

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

## Explicação do Código

A lógica do programa é construída sobre dois pilares principais: um **timer de alta precisão** para o controle do tempo e uma **comunicação segura entre a interrupção e o loop principal** para evitar condições de corrida (*race conditions*).

#### 1\. Configuração do Timer (`esp_timer`)

O coração do sistema de contagem é o `esp_timer`, um timer de alta resolução do ESP-IDF. Ele é configurado na função `app_main` da seguinte forma:

```c
const esp_timer_create_args_t timer_args = {
    .callback = &timer_isr, // Função a ser chamada quando o timer disparar
    .name = "Timer"
};
esp_timer_create(&timer_args, &timer_handler);
esp_timer_start_periodic(timer_handler, 1000000); // Inicia o timer para disparar a cada 1.000.000 µs (1 segundo)
```

  - Primeiro, definimos os argumentos de criação, especificando qual função (`timer_isr`) deve ser executada quando o timer "disparar".
  - Em seguida, criamos e iniciamos o timer no modo periódico, configurado para acionar a interrupção a cada 1 segundo.

#### 2\. A Rotina de Interrupção (ISR)

A função `timer_isr` é a nossa Rotina de Serviço de Interrupção. A regra de ouro para ISRs é que elas devem ser o mais **curtas e rápidas** possível. A única responsabilidade desta função é sinalizar para o resto do programa que um segundo se passou.

```c
void IRAM_ATTR timer_isr(void *arg)
{
    f_led = 1; // Aciona a flag para o loop principal
}
```

  - O atributo `IRAM_ATTR` garante que o código da ISR seja carregado na memória RAM interna do ESP32, tornando sua execução extremamente rápida.
  - A única ação é definir a *flag* `f_led` como `1`. A *flag* é declarada como `volatile` para garantir que o compilador não otimize seu acesso, pois ela é modificada em um contexto (interrupção) e lida em outro (loop principal).

#### 3\. O Loop Principal (`app_main`)

O loop `for(;;)` é o cérebro da aplicação. Ele tem duas responsabilidades principais:

1.  **Verificar a Flag e Gerenciar a Contagem:**

    ```c
    if (f_led){
      f_led = 0; // "Consome" o evento do timer
      count++;   // Incrementa o contador
      if (count >= 10){
        count = 0; // Zera o contador após o 9
      }
    }
    ```

      - O loop constantemente verifica se a `f_led` foi acionada.
      - Se sim, ele imediatamente a zera para que o incremento ocorra apenas uma vez por segundo.
      - Em seguida, ele atualiza a variável `count`, implementando a lógica de contagem cíclica.

2.  **Atualizar o Display:**

    ```c
    for (int i=0; i<7; i++) {
      gpio_set_level(i + 4, matriz[count][i]); 
    }
    ```

      - Esta parte do código é responsável por "desenhar" o número no display.
      - Ele itera sobre os 7 segmentos e usa a `matriz` de consulta para obter o padrão de bits correto para o valor atual de `count`, acendendo ou apagando cada pino GPIO correspondente.

Essa arquitetura desacoplada (ISR rápida e lógica no loop principal) é um padrão de design robusto e eficiente para sistemas embarcados.


