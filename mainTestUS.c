#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//void InicializarShield(void);
void configurarPinesUS(unsigned char pin_disparo, unsigned char pin_eco);
void inicializarUS(void);
float getMedidaUS(unsigned char pin_eco);


int main (void) {
    //InicializarShield();
    //InicializarReceptor();
    
    ANSELC &= 0xFFF0;
    TRISC &= 0xFFF0;
    LATC &= 0xFFF0;
    
    unsigned char pin_disparo = 1; //RA1
    unsigned char pin_eco = 0; //RA0

    float tiempo_vuelo;
    float distancia = 0.;

    configurarPinesUS(pin_disparo, pin_eco);
    inicializarUS();
    
    INTCONbits.MVEC=1;
    asm("ei");
    //ActivarCoche('F');
    
    
        
    while(1){
        
        tiempo_vuelo = getMedidaUS(pin_eco);
        distancia = 0.5*34000.*tiempo_vuelo*50.E-6;
        
        if (distancia < 20.) {
            LATCCLR = (15);
            //ActivarCoche('F');
        } else {
            LATCSET = (15);
            //ActivarCoche('S');
            
        }

        //ActivarCoche('8');
       
    }
}