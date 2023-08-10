#include <xc.h>
#include <stdint.h>

// ------------------------------------------------------
// --------------------- CONSTANTES ---------------------
// ------------------------------------------------------
#define NUM_PINES_US 16 // Número de pines US
#define TIEMPO_ESPERA 200 // En múltiplos de 50 us

// ------------------------------------------------------
// ------------ VARIABLES GLOBALES AL MÓDULO ------------
// ------------------------------------------------------
static unsigned int medida_us[] =
{511, 511, 511, 511, 511, 511, 511, 511,
511, 511, 511, 511, 511, 511, 511, 511};
static unsigned pin_configurado[] =
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char pines_disparo[NUM_PINES_US];
static unsigned char pin_us = 0;

// -----------------------------------------------------
// ---------------- FUNCIONES PÚBLICAS -----------------
// -----------------------------------------------------
void configurarPinesUS(unsigned char pin_disparo, unsigned char pin_eco) 
{
    if (pin_disparo >= NUM_PINES_US || pin_eco >= NUM_PINES_US)
        return;
    
    TRISACLR = (1 << pin_disparo); //Disparo: Salida
    TRISASET = (1 << pin_eco); // Eco: Entrada
    
    ANSELA &= ~((1<<pin_eco) | (1<<pin_disparo));
    
    LATACLR = 1 << pin_disparo;
    
    pines_disparo[pin_eco] = pin_disparo;
    pin_configurado[pin_eco] = 1;
    
    
}
// -----------------------------------------------------

void inicializarUS(void) 
{
    // Seleccionar el primer pin de eco configurado
    unsigned char i = 0;
    for (i = 0; i < NUM_PINES_US; i++) {
        if (pin_configurado[i] == 1) {
            pin_us = i;
            break;
        }
    }
    
    T3CON = 0x0000; // OFF, preescalado 1:1
    PR3 = 249; // Tiempo = 50 us
    TMR3 = 0; // Inicializar la cuenta
    IFS0bits.T3IF = 0; // Borrar la bandera
    IEC0bits.T3IE = 1; // Habilitar interrupciones
    IPC3bits.T3IP = 3; // Prioridad interrupciones
    T3CONbits.ON = 1; // Encender el Timer 3
}
// -----------------------------------------------------

float getMedidaUS(unsigned char pin_eco) {
    
    if (pin_eco >= NUM_PINES_US)
        return 511; //Si no se ha realizado la medida o no ha detectado un objeto devuelve 511
    return medida_us[pin_eco];
}

// -----------------------------------------------------
// ---------------- FUNCIONES PRIVADAS -----------------
// -----------------------------------------------------

__attribute__((vector(12),interrupt(IPL3SOFT),nomips16))
void InterrupcionTimer3(void)
{
    static unsigned char estado = 0;
    static unsigned int contador_espera = 0;
    static unsigned char eco_actual = 0;
    unsigned char eco_anterior;
    static unsigned int medida = 0;
    
    IFS0bits.T3IF = 0; // Borrar el flag
    
    switch (estado) {
    case 0: // Iniciar el disparo
        LATASET = 1 << pines_disparo[pin_us];
        estado = 1;
        break;
    case 1: // Finalizar el disparo
        LATACLR = (1 << pines_disparo[pin_us]);
        eco_actual = 0;
        estado = 2;
        break;
    case 2: // Esperar al inicio de la recepción
        eco_anterior = eco_actual;
        eco_actual = (PORTC >> pin_us) & 0x1;
        // Flanco de subida
        if (eco_actual > eco_anterior) {
            medida = 0;
            contador_espera = 0;
            estado = 3;
        }
        // Pasar al siguiente sensor si no llega el flanco.
        // Como ya se ha esperado aquí, no se reinicia el
        // contador para no volver a esperar en el
        // estado 4.
        else if (++contador_espera > TIEMPO_ESPERA) {
            estado = 4;
        }
        break;
    case 3: // Recibir la medida
        eco_anterior = eco_actual;
        eco_actual = (PORTA >> pin_us) & 0x1;
        // Limitar la medida máxima
        if (++medida > 511)
            medida = 511;
        // Flanco de bajada
        if (eco_actual < eco_anterior) {
            medida_us[pin_us] = medida;
            contador_espera = 0;
            estado = 4;
        }
        else if (++contador_espera > TIEMPO_ESPERA) {
            estado = 4;
        }
        break;
    case 4: // Esperar antes de la siguiente medida
        if (++contador_espera > TIEMPO_ESPERA) {
            // Buscar el siguiente sensor de ultrasonidos
            // ¡ATENCIÓN! Bloquea el programa si no hay
            // ningún pin US configurado

            do {
                if (++pin_us >= NUM_PINES_US)
                pin_us = 0;
            } while (pin_configurado[pin_us] == 0);

            contador_espera = 0;
            estado = 0;
        }
        break;
    }
}