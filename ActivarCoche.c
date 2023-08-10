
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <xc.h>
#include <math.h>
#include "Pic32Ini.h"

# define MOTOR_1_PWM 6 //RC7
# define MOTOR_2_PWM 9 //RC8
# define DIR_MOT_1 7;
# define DIR_MOT_2 8;
# define MSK_MOTOR 0x3C0
#define PERIODO 499

float getDistancia(void);

int conexion = 0; //1 - conectado, 0 - desconectado
float velocidad;
float distancia = 0.;


__attribute__((vector(8),interrupt(IPL2SOFT),nomips16))
void InterrupcionTimer2(void)
{
    IFS0bits.T2IF = 0;
    if (conexion == 0){ //OFF
        LATC &= ~(1<<MOTOR_1_PWM);
        LATC &= ~(1<<MOTOR_2_PWM);
    }

}

void InicializarShield(){

    ANSELC &= ~(MSK_MOTOR);
    TRISC &= ~(MSK_MOTOR);
    LATC |= MSK_MOTOR;
    
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    RPC8R = 5; // OC1 conectado a RC8 (Motor 2)
    RPC7R = 5; // OC2 conectado a RC7 (Motor 1)
    SYSKEY = 0x1CA11CA1;
    
    OC1CON = 0;
    OC1R = 0;
    OC1RS = 0;
    OC1CON = 0x8006;
    
    OC2CON = 0;
    OC2R = 0;
    OC2RS = 0;
    OC2CON = 0x8006;
    
    T2CON = 0;
    TMR2 = 0;
    IFS0bits.T2IF = 0;
    PR2 = PERIODO;

    IEC0bits.T2IE = 1;
    IPC2bits.T2IP = 2; 
    T2CON = 0x8000;
}

void ActivarCoche(char c){
    
    //putsUART(c);
    
    if (c !='\0'){

        conexion = 1; //Carácter recibido, conexión realizada

        //Velocidades (Los valores provienen de la app utilizada)
        if (c == '0'){
            velocidad = 0;
        }
        else if (c=='1'){
            velocidad = 0.1;
        }
        else if (c=='2'){
            velocidad = 0.2;
        }
        else if (c=='3'){
            velocidad = 0.3;
        }
        else if (c=='4'){
            velocidad = 0.4;
        }
        else if (c=='5'){
            velocidad = 0.5;
        }
        else if (c=='6'){
            velocidad = 0.6;
        }
        else if (c=='7'){
            velocidad = 0.7;
        }
        else if (c=='8'){
            velocidad = 0.8;
        }
        else if (c=='9'){
            velocidad = 0.9;
        }
        else if (c=='q'){
            velocidad = 1;
        }
        
        if (c == 'V') {
            EncenderBocina();
        } else if (c == 'v') {
            ApagarBocina();
        }
        
        if(c == 'F'){  //Forward
            
            OC1RS = PERIODO * velocidad;
            OC2RS = PERIODO * velocidad;
            LATC &= ~(1<<MOTOR_1_PWM);
            LATC &= ~(1<<MOTOR_2_PWM);
           
        }

        else if(c == 'B'){  //Backwards

            OC1RS = PERIODO * velocidad;
            OC2RS = PERIODO * velocidad;
            LATC |= (1<<MOTOR_1_PWM);
            LATC |= (1<<MOTOR_2_PWM);

        }
        
        else if(c == 'S'){ //Stop

            OC1RS = 0;
            OC2RS = 0;

        }

    }else{
        conexion = 0;
    }
    
    
}