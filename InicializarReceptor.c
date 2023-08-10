/*# include <xc.h>
# include <stdlib.h>

# define PIN 13 //Pin de recepción a RB13

void InicializarReceptor(void) {

    ANSELB &= ~(1<<PIN);

    TRISB |= (1<<PIN);

    LATB |= (1<<PIN); //Solución al problema de envío de primeros carácteres erróneos

    SYSKEY=0xAA996655; //Desbloqueamos registros de configuraci�n
    SYSKEY=0x556699AA;

    U1RXR = 3; //Pin remapping

    SYSKEY=0x1CA11CA1; //Volvemos a bloquearlos

    U1BRG = 32;

    U1STAbits.URXEN = 1; //Enable receptor
    U1MODE = 0x8000; //UART ON con 8N1
}*/