/*
 * File:   main.c
 * Author: diego
 *
 * Created on 25 de septiembre de 2021, 09:48 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h> 
#define _XTAL_FREQ 1000000 // Frecuencia por default 
void main(void) { 
    
    // TRISD=0;     // Original
    // ANSELD=0;    // Original
    TRISA = 0;   // Ejercicio 1 y 2
    ANSELA = 0;  // Ejercicio 1 y 2

    char contador = 0xFF;
    while(1) 
    { 
        // LATDbits.LD1 ^= 1;   // Original
        // LATAbits.LA1 ^= 1;   // Ejercicio 1
        LATA = contador;
        __delay_ms(500);
        __delay_ms(500);    // Ejercicio 3
        contador--;
        if(contador == 0) {
            contador = 0xFF;
        }
    } 
    return; 
}