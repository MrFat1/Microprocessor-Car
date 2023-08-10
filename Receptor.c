# include <stdio.h>
# include <stdlib.h>
# include <xc.h>

# define TAM_COLA 100
# define MSK_PIN 0x2080 //Pines RB13 y RB7

static char cola_rx[TAM_COLA];
static int icab_rx = 0;
static int icol_rx = 0;

static char cola_tx[TAM_COLA];
static int icab_tx = 0;
static int icol_tx = 0;


void InicializarUART1(int baudios){
    
    if (baudios < 38400) {
        U1BRG = (5000000) / (16*baudios) - 1; //Lento
    } else {
        U1BRG = (5000000) / (4*baudios) - 1; //R?pido
    }

    IFS1bits.U1RXIF = 0; //Interrupci?n en el receptor
    IEC1bits.U1RXIE = 1; //Interruption enable
    IFS1bits.U1TXIF = 0; //Interrupci?n en el transmisor

    IPC8bits.U1IP = 3;
    IPC8bits.U1IS = 1;

    ANSELB &= ~(MSK_PIN);
    TRISB |= 1<<13;
    LATB |= (MSK_PIN); //Ponemos ambos a uno par solucionar un error de transmisión

    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    U1RXR = 3; //RB13
    RPB7R = 1; //RB7
    SYSKEY = 0x1CA11CA1;

    U1STAbits.URXISEL = 0; //Al menos un car?cter en la FIFO
    U1STAbits.UTXISEL = 2; //Interrumpte si la FIFO esta vac?a

    //Enables
    U1STAbits.URXEN = 1;
    U1STAbits.UTXEN = 1;

    U1MODE = 0x8000; //UART ON
    
}

__attribute__((vector(32), interrupt(IPL3SOFT), nomips16))
void InterrupcionUART1(void) {

    if (IFS1bits.U1RXIF == 1) { //Interrupci?n del receptor
        if ((icab_rx + 1 == icol_rx) || (icab_rx + 1 == TAM_COLA && icol_rx == 0)) {
            
        } else {
            cola_rx[icab_rx] = U1RXREG; //Leemos el car?cter
            icab_rx++;
            if (icab_rx == TAM_COLA) { //Llegamos arriba
                icab_rx = 0;
            }
        }

        IFS1bits.U1RXIF = 0;

    }

    if (IFS1bits.U1TXIF == 1) { //Transmisor interrumpe
        if (icol_tx != icab_tx) {
            U1TXREG = cola_tx[icol_tx];
            icol_tx++;
            if (icol_tx == TAM_COLA) {
                icol_tx = 0;
            }
        } else { //Cola vacia
            IEC1bits.U1TXIE = 0; //Apagamos el transmisor
        }

        IFS1bits.U1TXIF = 0;
    }
}


void putsUART(char s[]){
    
    if (s != NULL) {
        int i=0;
        while (s[i] != '\0') {

            if ((icab_tx + 1 == icol_tx) || (icab_tx + 1 == TAM_COLA && icol_tx == 0)) {
                break; //Cola llena, aborta envio de caracteres
            } else {
                cola_tx[icab_tx] = s[i];
                i++;
                icab_tx++;
                if (icab_tx == TAM_COLA) {
                    icab_tx = 0;
                }
            }
        }

        IEC1bits.U1TXIE = 1;
    }
    
}

char getcUART(void){
    char c;

    if (icol_rx != icab_rx) {
        c = cola_rx[icol_rx];
        icol_rx ++;
        if (icol_rx == TAM_COLA) {
            icol_rx = 0;
        }
    } else { //No ha llegado nada
        c = '\0';
    }

    return c;
}