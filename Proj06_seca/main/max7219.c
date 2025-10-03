#include "max7219.h"

static void max7219_spi_init(void) {
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

static inline esp_err_t max7219_write(uint8_t reg, uint8_t val) {
  spi_transaction_t SpiTransaction  = {
    .flags = SPI_TRANS_USE_TXDATA,
    .length = 16,                // 16 bits: reg (8) + val (8)
    .tx_data = { reg, val, 0, 0 }
  };
  return spi_device_polling_transmit(SpiDeviceHandle, &SpiTransaction );
}

static void max7219_init_chip(void) {
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

// Helper: acende (ou apaga) uma linha inteira
static inline void set_line(uint8_t line_idx, bool on) {
  // line_idx: 0..7  (DIGIT0..DIGIT7)
  // valor 0xFF = 8 LEDs acesos na linha; 0x00 = tudo apagado
  max7219_write(REG_DIGIT0 + (line_idx & 0x07), on ? 0xFF : 0x00);
}

void draw_digit(uint8_t d) {
    if (d > 9) return;
    for (int row = 0; row < 8; row++) {
        max7219_write(0x01 + row, DIGITS_8x8[d][row]);
    }
}
