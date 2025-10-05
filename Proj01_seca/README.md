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

\<div\>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/b29ed7ee-ad37-4379-9e93-1637aa2c799c" />
\</div\>
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

## Explicação do Código

A arquitetura do software baseia-se em um modelo **orientado a eventos**, utilizando uma interrupção de GPIO para detectar o toque no botão. A comunicação entre a rotina de interrupção (ISR) e o loop principal é feita de forma segura através de uma *flag*.

#### 1\. Configuração da Interrupção

Na função `app_main`, o pino do botão é configurado como entrada com um resistor de *pull-up* interno. Em seguida, o serviço de interrupção do ESP32 é inicializado e a nossa função `button_isr` é "anexada" ao pino do botão.

```c
// --- Configuração do pino do botão ---
gpio_set_direction(GPIO_NUM_1, GPIO_MODE_INPUT);
gpio_set_pull_mode(GPIO_NUM_1, GPIO_PULLUP_ENABLE);
gpio_set_intr_type(GPIO_NUM_1, GPIO_INTR_LOW_LEVEL);

// --- Configuração do serviço de interrupção ---
gpio_install_isr_service(0);
gpio_isr_handler_add(GPIO_NUM_1, button_isr, NULL);
```

Isso faz com que a função `button_isr` seja chamada automaticamente pelo hardware sempre que o pino `GPIO 1` for para nível baixo (ou seja, quando o botão for pressionado).

#### 2\. A Rotina de Interrupção (ISR)

A função `button_isr` é a nossa Rotina de Serviço de Interrupção. Para garantir a estabilidade do sistema, ela é o mais **curta e rápida** possível. Sua única tarefa é sinalizar para o loop principal que o botão foi pressionado.

```c
void IRAM_ATTR button_isr()
{
    f_button = 1; // Aciona a flag para o loop principal
}
```

  - O atributo `IRAM_ATTR` garante que este código seja executado a partir da memória RAM interna, que é mais rápida e sempre acessível.
  - A *flag* `f_button` é declarada como `volatile` para garantir que o compilador sempre leia seu valor da memória, pois ele pode ser alterado a qualquer momento pela interrupção.

#### 3\. O Loop Principal e a Lógica de Contagem

O `for(;;)` em `app_main` executa um loop infinito que realiza duas tarefas:

1.  **Verificar a Flag e Gerenciar a Contagem:**

    ```c
    if (f_button) {
        f_button = 0; // "Consome" o evento
        count = (count + 1) % 10; // Incrementa o contador
    }
    ```

      - O loop verifica constantemente se a `f_button` foi acionada pela ISR.
      - Se sim, ele primeiro a zera para não contar o mesmo toque múltiplas vezes.
      - Em seguida, ele atualiza a variável `count` com a lógica de contagem cíclica (0 a 9).

2.  **Atualizar o Display:**

    ```c
    display_set(segs, count);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ```

      - A função `display_set` é chamada em todas as iterações para garantir que o display sempre mostre o valor atual de `count`.
      - O `vTaskDelay` introduz uma pausa, o que ajuda a economizar processamento e também funciona como um *debounce* simples, evitando que múltiplos acionamentos da interrupção (se o botão vibrar) sejam contados dentro de um curto intervalo.

