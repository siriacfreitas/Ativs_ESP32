// --- Guarda de Inclusão (Header Guard) ---
// Garante que o conteúdo deste arquivo seja incluído apenas uma vez na compilação.
#ifndef DISPLAY_H
#define DISPLAY_H

// --- Declaração Externa da Matriz de Consulta ---
extern const int matriz[10][7];

// --- Protótipos das Funções ---
// Configura os pinos GPIO dos segmentos como saídas.
void display_init(const int *segs);
// Exibe um dígito no display de 7 segmentos.
void display_set(const int *segs, int count);

#endif /* DISPLAY_H */