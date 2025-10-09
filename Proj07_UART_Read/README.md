# Projeto 07: Display de 7 Segmentos Controlado via UART

## Questão Proposta

**Disciplina:** EGM0029 - Sistemas Embarcados para Controle e Automação  
**Período:** 2025.2  
**Projeto:** 03

> 2- Elabore um código fonte para ler um dado entre 0 e 9 no terminal, através de uma conexão serial, e exibi-lo em um display de sete segmentos.

## Descrição do Projeto

Este projeto transforma um ESP32 em um "display remoto". O sistema aguarda o recebimento de um caractere via **comunicação serial (UART)** a partir de um computador. Ao receber um dado, o código valida se é um dígito numérico de '0' a '9'. Em caso afirmativo, o número correspondente é exibido em um display de 7 segmentos do tipo **Ânodo Comum**.

A lógica para acender os segmentos corretos para cada número está implementada através de uma matriz de consulta (*lookup table*). O programa também envia feedback de volta ao terminal serial, informando o caractere que foi recebido, criando uma interface de usuário interativa e simples.

## Funcionalidades

  - Recebe dados de um computador via comunicação serial (UART).
  - Filtra e valida a entrada para processar apenas os dígitos de '0' a '9'.
  - Exibe o número recebido em um display de 7 segmentos.
  - Envia mensagens de prompt e feedback para o terminal do usuário.
  - Código modularizado, com a lógica do display separada da lógica principal.

## Hardware Utilizado

  - 1x Placa de Desenvolvimento ESP32
  - 1x Display de 7 Segmentos (modelo Ânodo Comum)
  - 7x Resistores de \~220Ω (para proteger os segmentos do display)
  - 1x Cabo USB (para alimentação e comunicação UART com o PC)
  - Jumpers para as conexões
  - 1x Protoboard

## Esquema de Conexões
<div>
<img width="350" height="350" alt="Image" src="https://github.com/user-attachments/assets/3f156f51-0ad2-4348-9013-f7021b77fddc" />
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

| Interface | Pino no ESP32 | Descrição                                  |
| :-------- | :------------ | :----------------------------------------- |
| UART0 TX  | `GPIO 43`     | Conectado ao RX do conversor USB-Serial    |
| UART0 RX  | `GPIO 44`     | Conectado ao TX do conversor USB-Serial    |
*Nota: Os pinos da UART podem variar dependendo do modelo do seu ESP32. Verifique o esquemático da sua placa.*

## Explicação do Código

A arquitetura do software é baseada em um modelo de **polling**, onde o loop principal verifica ativamente (sondagens) se há novos dados disponíveis no buffer da UART.

#### 1\. Configuração da UART

Na função `app_main`, a interface UART é configurada com os parâmetros de comunicação padrão (9600 bps, 8N1) e os pinos de hardware para transmissão (TX) e recepção (RX) são definidos.

```c
// --- Configuração dos parâmetros da UART ---
uart_config_t uart_config = {
    .baud_rate  = 9600,
    .data_bits  = UART_DATA_8_BITS,
    .parity     = UART_PARITY_DISABLE,
    .stop_bits  = UART_STOP_BITS_1,
    .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
};

// --- Instalação do driver e definição dos pinos ---
uart_driver_install(0, BUF_SIZE * 2, 0, 0, NULL, 0);
uart_param_config(0, &uart_config);
uart_set_pin(0, GPIO_NUM_43, GPIO_NUM_44, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
```

#### 2\. O Loop Principal e a Leitura de Dados

O `for(;;)` em `app_main` executa um loop infinito que gerencia a interação com o usuário.

1.  **Enviar o Prompt:**
    A cada iteração, uma mensagem é enviada ao terminal para instruir o usuário.

    ```c
    const char* msg = "Escreva um número de 0 a 9: \r\n";
    uart_write_bytes(0, msg, strlen(msg));
    ```

2.  **Aguardar e Ler a Entrada:**
    A função `uart_read_bytes` é chamada para verificar se há dados no buffer de recepção. Ela é configurada com um *timeout*, fazendo com que o programa espere por um tempo determinado antes de continuar. Isso é conhecido como **leitura bloqueante com timeout**.

    ```c
    len = uart_read_bytes(0, data, 1, 3000/portTICK_PERIOD_MS);
    ```

    *Nota: É fundamental que o tamanho da leitura (`1` neste caso) seja compatível com o buffer de destino para evitar erros de `buffer overflow`.*

#### 3\. Validação, Conversão e Exibição

Após a leitura, o código processa os dados recebidos.

```c
// Se algum dado foi recebido...
if (len) {
    data[len] = '\0'; // Transforma em string C válida
    printf("Recebido: %s\n", (char *) data); // Ecoa a entrada no terminal
    
    // Valida se o caractere recebido é um dígito.
    if (data[0] >= '0' && data[0] <= '9') {
        // Converte o caractere ASCII para um número inteiro (ex: '5' -> 5).
        int numero = data[0] - '0';
        // Atualiza o display com o novo número.
        display_set(segs, numero);
    }
}
```

  - Primeiro, o código verifica se `len` é maior que zero, ou seja, se algum caractere foi realmente lido.
  - Em seguida, ele valida se o caractere está no intervalo de '0' a '9'.
  - O passo mais importante é a conversão `int numero = data[0] - '0';`. Como a UART envia o valor ASCII dos caracteres (ex: '5' é 53), subtrair o valor ASCII de '0' (que é 48) resulta no valor numérico inteiro correspondente.
  - Finalmente, a função `display_set` é chamada para exibir o número no display. O `vTaskDelay` ao final do loop garante que a CPU tenha um tempo de descanso entre as iterações.