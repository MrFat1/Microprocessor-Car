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
void configurarPinesUS(unsigned char pin_disparo, unsigned char pin_eco);
void inicializarUS(void);
float getMedidaUS(unsigned char pin_eco);
void InicializarShield(void);
char getcUART(void);

int main (void) {

    char cadena[8];

    InicializarUART1(9600);

    //unsigned char pin_disparo = 1; //RA1
    //unsigned char pin_eco = 0; //RA0

    //float tiempo_vuelo;
    //float distancia = 0.;

    //configurarPinesUS(pin_disparo, pin_eco);
    //inicializarUS();
    
    InicializarShield();
    IniciarBocina();
    
    INTCONbits.MVEC=1;
    asm(" ei");

    enum {AnalizarComando, Bocina, Marcha} estado;

    estado = AnalizarComando;
    
    while(1){

        /*tiempo_vuelo = getMedidaUS(pin_eco);
        distancia = 0.5*34000.*tiempo_vuelo*50.E-6;*/
        
        
        if ((cadena[0] = getcUART())!=0) {
            cadena[1] = 0;
        }
        //putsUART(cadena);

        switch(estado) {

            case AnalizarComando:

                if (c == 'V' || c =='v') {

                    estado = Bocina;

                }else {

                    estado = Marcha;

                }

                break;

            case Bocina:

                if (c == 'V') { //Bocina on
                    EncenderBocina();
                } else if (c == 'v') { //Bocina off
                    ApagarBocina();
                }
                
                estado = AnalizarComando;
                break;

            case Marcha:
                

                ActivarCoche(c);
                
                estado = AnalizarComando;

                break;

        }



    }

}