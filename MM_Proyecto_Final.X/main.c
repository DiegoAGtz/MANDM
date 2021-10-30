/*
 * File:   tx1.c
 * Author: Administrator1
 *
 * Created on 27 de septiembre de 2021, 08:57 AM
 */
#pragma config FOSC = INTOSCIO  // Oscillator Selection (Internal oscillator)
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored))
#pragma config MCLRE = ON       // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>

#define _XTAL_FREQ 4000000      // Frecuencia del micro

void Configuracion(void);

void main(void) {
    char A = 65;
    Configuracion();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    while (1) {
        PORTA = A;
        TXREG1 = A;
        __delay_ms(500);
        A += 1;
        if (A > 90) A = 65;
    }

    return;
}

void Configuracion(void) {
    TRISA = 0;
    TRISC = 0xC0; // b?11000000?
    ANSELA = 0;
    ANSELC = 0;

    OSCCON = 0x53; //4MHz

    SPBRG1 = 25;
    TXSTA1 = 0x24;
    SPBRGH1 = 0x00;
    RCSTA1 = 0x90;
}
