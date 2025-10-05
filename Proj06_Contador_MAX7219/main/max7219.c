
#include "max7219.h"

// --- DEFINIÇÃO DA VARIÁVEL GLOBAL ---
spi_device_handle_t  SpiDeviceHandle; // Esta é a única e verdadeira definição

/********************************************************************************************
 * Function name : void max7219_spi_init(void)
 * returns       : Nada (void)
 * Description   : Configura e inicializa o barramento SPI (SPI2_HOST) para comunicação 
 *                 com o CI MAX7219. Define os pinos MOSI, CLK e CS, e adiciona o MAX7219
 *                 como um dispositivo no barramento com as configurações apropriadas 
 *                 (velocidade do clock, modo SPI 0).
 * Notes         : O pino MISO não é utilizado pois o MAX7219 não envia dados de volta
 *                 para o microcontrolador (comunicação half-duplex).
 ********************************************************************************************/

void max7219_spi_init(void) {
  spi_bus_config_t  SpiBusConfig = {
    .mosi_io_num = MAX7219_DIN,
    .miso_io_num = -1,               // MAX7219 não usa MISO
    .sclk_io_num = MAX7219_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
  };
 spi_bus_initialize(SPI2_HOST, &SpiBusConfig, SPI_DMA_CH_AUTO);

  spi_device_interface_config_t SpiDeviceConfig = {
    .clock_speed_hz = 10000000,  
    .mode = 0,                         // CPOL=0, CPHA=0
    .spics_io_num = MAX7219_CS,        // LOAD/CS
    .queue_size = 2,
    .flags = SPI_DEVICE_HALFDUPLEX,    // só TX
  };
  spi_bus_add_device(SPI2_HOST, &SpiDeviceConfig, &SpiDeviceHandle); 
}

/********************************************************************************************
 * Function name : esp_err_t max7219_write(uint8_t reg, uint8_t val)
 * returns       : esp_err_t - Retorna ESP_OK em caso de sucesso ou um código de
 *                 erro em caso de falha na transmissão.
 * reg           : O endereço do registrador do MAX7219 (8 bits) ao qual se deseja escrever.
 * val           : O valor (8 bits) a ser escrito no registrador especificado.
 * Description   : Envia um comando de 16 bits (endereço do registrador + valor)
 *                 para o MAX7219 através do barramento SPI. Utiliza uma transação 
 *                 SPI síncrona (polling) para enviar os dados.
 * Notes         : A função é declarada como 'static inline' para otimização, sugerindo
 *                 ao compilador que insira o código diretamente no local da chamada,
 *                 evitando a sobrecarga de uma chamada de função.
 ********************************************************************************************/

 esp_err_t max7219_write(uint8_t reg, uint8_t val) {
  spi_transaction_t SpiTransaction  = {
    .flags = SPI_TRANS_USE_TXDATA,
    .length = 16,                // 16 bits: reg (8) + val (8)
    .tx_data = { reg, val, 0, 0 }
  };
  return spi_device_polling_transmit(SpiDeviceHandle, &SpiTransaction );
}

/********************************************************************************************
 * Function name : void max7219_init_chip(void)
 * Description   : Inicializa o CI MAX7219 com uma configuração padrão para operação
 *                 com uma matriz de LED 8x8. Configura o modo de operação, desliga o
 *                 modo de teste, define o modo 'no-decode', ajusta o scan limit
 *                 para todas as 8 linhas, define a intensidade do brilho e limpa o display.
 * Notes         : Utiliza a macro ESP_ERROR_CHECK para garantir que cada comando de
 *                 escrita seja bem-sucedido. Se algum falhar, o programa irá parar.
 ********************************************************************************************/

void max7219_init_chip(void) {
  ESP_ERROR_CHECK(max7219_write(REG_SHUTDOWN,    0x01)); // normal operation
  ESP_ERROR_CHECK(max7219_write(REG_DISPLAYTEST, 0x00)); // display test off
  ESP_ERROR_CHECK(max7219_write(REG_DECODE_MODE, 0x00)); // sem decode (matriz)
  ESP_ERROR_CHECK(max7219_write(REG_SCAN_LIMIT,  0x07)); // 8 linhas (0..7)
  ESP_ERROR_CHECK(max7219_write(REG_INTENSITY,   0x08)); // brilho médio (0..0x0F)

  // limpa todas as linhas
  for (uint8_t row = 0; row < 8; row++) {
    ESP_ERROR_CHECK(max7219_write(REG_DIGIT0 + row, 0x00));
  }
}

/********************************************************************************************
 * Function name : void set_line(uint8_t line_idx, bool on)
 * line_idx      : O índice da linha (0 a 7) que será modificada.
 * on            : Um valor booleano. 'true' para acender todos os LEDs da linha,
 *                 'false' para apagar.
 * Description   : Função auxiliar para acender ou apagar todos os 8 LEDs de uma
 *                 linha específica da matriz. Escreve 0xFF (para acender) ou
 *                 0x00 (para apagar) no registrador correspondente à linha.
 * Notes         : O operador '& 0x07' garante que o índice da linha permaneça
 *                 dentro do intervalo válido [0, 7], prevenindo acessos a
 *                 registradores inválidos.
 ********************************************************************************************/
void set_line(uint8_t line_idx, bool on) {
  // line_idx: 0..7  (DIGIT0..DIGIT7)
  // valor 0xFF = 8 LEDs acesos na linha; 0x00 = tudo apagado
  max7219_write(REG_DIGIT0 + (line_idx & 0x07), on ? 0xFF : 0x00);
}

/********************************************************************************************
 * Function name : void draw_digit(uint8_t d)
 * d             : O dígito (0 a 9) que será desenhado na matriz de LED.
 * Description   : Desenha um dígito numérico na matriz de LED 8x8. A função
 *                 itera sobre as 8 linhas, enviando para cada uma o padrão
 *                 de bits correspondente ao dígito, que é obtido de uma
 *                 matriz 'lookup table' chamada DIGITS_8x8.
 * Notes         : Depende de uma matriz global ou estática `DIGITS_8x8[10][8]` que
 *                 deve ser definida em outro lugar no código, contendo os
 *                 padrões de bitmap para cada dígito.
********************************************************************************************/

void draw_digit(uint8_t d) {
    if (d > 9) return;
    for (int row = 0; row < 8; row++) {
        max7219_write(0x01 + row, DIGITS_8x8[d][row]);
    }
}
