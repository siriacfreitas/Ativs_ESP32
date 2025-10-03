#ifdef DISPLAY.H
#ifndef DISPLAY.H

#define DISPLAY.H //obs colocar para n chamar dnv

/* o vetor segue na ordem do a ao f e as colunas vão do 0 ao 9, apagando no 1 e ligando no 0 */
const int matriz[10][7]={   {0,0,0,0,0,0,1}, 
                            {1,0,0,1,1,1,1},
                            {0,0,1,0,0,1,0},
                            {0,0,0,0,1,1,0},
                            {1,0,0,1,1,0,0}, 
                            {0,1,0,0,1,0,0},
                            {0,1,0,0,0,0,0},
                            {0,0,0,1,1,1,1},
                            {0,0,0,0,0,0,0},
                            {0,0,0,0,1,0,0}};



void display_init(int *segs);
void display_set(int *segs,int count);

#endif

