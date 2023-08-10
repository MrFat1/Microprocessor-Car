#include <xc.h>
#include <stdint.h>

# define PERIODO 5000 // Periodo de la señal de la bocina (1 ms - 1 KHz)
# define PIN_BOCINA 14 //RC4

void IniciarBocina(void){
    
    ANSELB &= ~(1<<PIN_BOCINA);
    TRISB &= ~(1<<PIN_BOCINA);

    SYSKEY=0xAA996655;
    SYSKEY=0x556699AA;
    RPB14R = 5; // OC1 conectado a RC7
    SYSKEY=0x1CA11CA1;

    OC3CON = 0;
    OC3R = 2500;
    OC3RS = 2500;
    OC3CON = 0x000E; // OC ON, modo PWM sin faltas, OCTSEL = 1 (Timer 3)

    T3CON = 0;
    TMR3 = 0;
    PR3 = PERIODO;
    IFS0bits.T3IF = 0;
    
    T3CON = 0x8000; // T2 ON, Div = 1


}


void ApagarBocina(void) {

    OC3CONbits.ON = 0;

}
void EncenderBocina(void)
{
    OC3CONbits.ON = 1;
}