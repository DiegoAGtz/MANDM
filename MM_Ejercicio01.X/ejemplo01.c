/*
 * File:   ejemplo01.c
 * Author: diego
 *
 * Created on 19 de agosto de 2021, 12:21 PM
 */

#pragma config FOSC = INTOSCIO  // Se selecciona oscilador interno
#pragma config WDTEN = OFF      // Se deshabilita el perro guardian
#pragma config MCLRE = ON       // Se deshabilita el pin 1 como reset externo
#pragma config LVP = OFF        // Se deshabilita la programación por bajo voltaje
#pragma config ICPRT = OFF      // Se deshabilita el pin para programación y depuración

#include <xc.h>                 // Archivo de cabecera para todos los programas

#define _XTAL_FREQ 1000000      // Constante que indica la frecuencia a la que opera el microcontrolador

void main(void) {
    TRISA = 0;      // Registro para definir los pines del puerto A como salidas (todos como 0 -> 0, todos como 1 -> 255, 8 pines)
    ANSELA = 0;     // Registro para definir los pines del puerto A como digitales (todos como 0)
    
    while(1) {
        LATAbits.LA0 ^= 1;  // Con cada ejecución ese pin cambia de 0 a 1 y de 1 a 0
        __delay_ms(500);    // Función de retardo en milisegundos
    }
    return;
}
