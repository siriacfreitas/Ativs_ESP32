// --- Guarda de Inclusão (Header Guard) ---
// Evita que este arquivo seja incluído mais de uma vez durante a compilação,
// o que causaria erros de redefinição.
#ifndef MAX7219_H
#define MAX7219_H

#include <stdio.h>
#include "freertos/FreeRTOS.h" // Sistema operacional de tempo real
#include "freertos/task.h"     // Gerenciamento de tarefas
#include "driver/spi_master.h" // Comunicação SPI para o display
#include "driver/gpio.h"       // Controle dos pinos de entrada/saída
#include "esp_err.h"           // Tratamento de erros do ESP-IDF

// --- Definição dos Pinos ---
// Mapeia as conexões físicas do MAX7219 aos pinos GPIO do ESP32.
#define MAX7219_DIN   GPIO_NUM_12 // Pino de Dados (Data In)
#define MAX7219_CLK   GPIO_NUM_11 // Pino de Clock (Clock)
#define MAX7219_CS    GPIO_NUM_10 // Pino de Seleção (Chip Select / Load)


// --- Mapa de Registradores do MAX7219 ---
// Enumeração que associa nomes legíveis aos endereços dos registradores,
// conforme o datasheet do componente.
enum {
  REG_NOOP        = 0x00, // Nenhuma operação
  REG_DIGIT0      = 0x01, // Linha 0
  REG_DIGIT1      = 0x02, // Linha 1
  REG_DIGIT2      = 0x03, // Linha 2
  REG_DIGIT3      = 0x04, // Linha 3
  REG_DIGIT4      = 0x05, // Linha 4
  REG_DIGIT5      = 0x06, // Linha 5
  REG_DIGIT6      = 0x07, // Linha 6
  REG_DIGIT7      = 0x08, // Linha 7
  REG_DECODE_MODE = 0x09, // Modo de decodificação
  REG_INTENSITY   = 0x0A, // Intensidade do brilho
  REG_SCAN_LIMIT  = 0x0B, // Limite de varredura (quantas linhas usar)
  REG_SHUTDOWN    = 0x0C, // Modo de desligamento/baixo consumo
  REG_DISPLAYTEST = 0x0F, // Modo de teste do display
};

// --- Fonte de Caracteres (Bitmap) 8x8 ---
// Matriz que contém os padrões de bytes para desenhar os dígitos de 0 a 9.
// Cada byte representa uma linha de 8 LEDs.
static const uint8_t DIGITS_8x8[10][8] = {
    // 0
    {0x3C, 0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x3C},
    // 1
    {0x08, 0x0C, 0x0A, 0x08, 0x08, 0x08, 0x08, 0x3E},
    // 2
    {0x3C, 0x42, 0x40, 0x30, 0x0C, 0x02, 0x02, 0x7E},
    // 3
    {0x3C, 0x42, 0x40, 0x38, 0x40, 0x40, 0x42, 0x3C},
    // 4
    {0x30, 0x28, 0x24, 0x22, 0x7E, 0x20, 0x20, 0x70},
    // 5
    {0x7E, 0x02, 0x02, 0x3E, 0x40, 0x40, 0x42, 0x3C},
    // 6
    {0x38, 0x04, 0x02, 0x3E, 0x42, 0x42, 0x42, 0x3C},
    // 7
    {0x7E, 0x42, 0x20, 0x10, 0x10, 0x08, 0x08, 0x08},
    // 8
    {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C},
    // 9
    {0x3C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x20, 0x1C}
};

// --- Protótipos das Funções ---
// Declara as funções que serão definidas no arquivo .c correspondente.
void max7219_spi_init(void);
esp_err_t max7219_write(uint8_t reg, uint8_t val);
void max7219_init_chip(void);
void set_line(uint8_t line_idx, bool on);
void draw_digit(uint8_t d);

#endif /* MAX7219_H */