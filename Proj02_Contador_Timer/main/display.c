#include "display.h"
#include <driver/gpio.h>

// --- Matriz de Consulta para Display de 7 Segmentos (Ânodo Comum) ---
// Mapeia os dígitos de 0 a 9 para o estado de cada segmento (a, b, c, d, e, f, g).
// Lógica: 0 = Segmento LIGADO (nível baixo), 1 = Segmento DESLIGADO (nível alto).
const int matriz[10][7] = {
    {0,0,0,0,0,0,1}, // 0
    {1,0,0,1,1,1,1}, // 1
    {0,0,1,0,0,1,0}, // 2
    {0,0,0,0,1,1,0}, // 3
    {1,0,0,1,1,0,0}, // 4
    {0,1,0,0,1,0,0}, // 5
    {0,1,0,0,0,0,0}, // 6
    {0,0,0,1,1,1,1}, // 7
    {0,0,0,0,0,0,0}, // 8
    {0,0,0,0,1,0,0}  // 9
};

/********************************************************************************************
 * Function name : void display_init(const int *segs)
 * returns       : Nada (void)
 * Description   : Configura os 7 pinos GPIO, fornecidos pelo array 'segs', como saídas 
 *                 digitais. Esta função deve ser chamada uma vez para preparar os pinos 
 *                  que controlarão os segmentos do display.
 * Notes         : O argumento 'segs' deve ser um array de 7 posições contendo os números
 *                 dos pinos GPIO conectados aos segmentos do display (a, b, c, d, e, f, g).
 ********************************************************************************************/
void display_init(const int *segs){
    for(int i = 0; i < 7; i++){
        gpio_set_direction(segs[i], GPIO_MODE_OUTPUT);  
    }
}

/********************************************************************************************
 * Function name : void display_set(const int *segs, int count)
 * returns       : Nada (void)
 * Description   : Acende ou apaga os segmentos do display de 7 segmentos para formar o 
 *                 dígito correspondente ao valor de 'count'. A função utiliza uma matriz 
 *                 de consulta (lookup table) chamada 'matriz' para obter o padrão de 
 *                 bits correto para cada dígito.
 * Notes         : Esta função depende de uma matriz global `matriz[10][7]` que deve ser 
 *                 definida em outro lugar. O argumento 'count' deve estar no intervalo 
 *                 dos dígitos definidos na matriz (geralmente 0-9).
 ********************************************************************************************/
void display_set(const int *segs, int count){
    for (int i = 0; i < 7; i++) {
        gpio_set_level(segs[i], matriz[count][i]); 
    }
}