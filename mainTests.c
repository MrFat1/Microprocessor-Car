#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


void InicializarUART1(int baudios);
void IniciarBocina(void);
void EncenderBocina(void);
void ApagarBocina(void);
void ActivarCoche(char c);
void IniciarBocina(void);
void InicializarShield(void);
char getcUART(void);


int main (void) {
    
    char cadena[8];
    
    InicializarShield();
    INTCONbits.MVEC = 1;
    InicializarUART1(9600);
    
    IniciarBocina();
    asm("ei");
    
    while(1){
        
        
        if ((cadena[0] = getcUART())!=0) {
            cadena[1] = 0;
            
            ActivarCoche(cadena[0]);
            putsUART(cadena);
        }
        
    }
}