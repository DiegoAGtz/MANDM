/*
 * File:   ejemplo02.c
 * Author: diego
 *
 * Created on 23 de agosto de 2021, 12:19 PM
 */

/*
 * Palabras de configuración, sirven como la primera etapa de configuración del
 * microcontrolador.
 * Son específicas de cada microcontrolador.
 */

#pragma config FOSC = INTOSCIO  // Se selecciona oscilador interno.
#pragma config WDTEN = OFF      // Se deshabilita el perro guardian (registro contador).
#pragma config MCLRE = ON       // Se deshabilita el pin 1 como reset externo.
#pragma config LVP = OFF        // Se deshabilita la programación por bajo voltaje.
#pragma config ICPRT = OFF      // Se deshabilita el pin para programación y depuración.

#include <xc.h> // Archivo cabecera para todos los programas.

#define _XTAL_FREQ 250000  // Constante que indica la frecuencia a la que opera el microcontrolador.

void main(void) {
    int conta = 240;
    
    OSCCON = 0x10;  // 0b00010000
    TRISD = 0;      // Registro para definir los pines del puerto D como salidas (todos como 0).
    ANSELD = 0;     // Registro para definir los pines del puerto D como digitales (todos como 0).
    
    TRISB = 0;
    ANSELB = 0;
    
    LATD = 240;
    while(1) {
        //LATD = conta;
        LATD += 1;
        LATB = conta / 256;
                
        __delay_ms(500);    // Función de retardo en milisegundos.
        conta += 1;
    }
    return;
}
