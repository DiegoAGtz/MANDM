/*
 * File:   msin.c
 * Author: diego
 *
 * Created on 28 de septiembre de 2021, 04:25 PM
 */

// Program using TMR0 timer and a lookup table for operating a 7-segment display 
#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>
#include <pic18f45k50.h> 

#define _XTAL_FREQ 1000000  // Frecuencia por defaul

char Value;
void __interrupt(high_priority) myHiIsr(void);
void configuracion(void);

void main(void) {
    const char Dis7seg[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x73};
    configuracion();

    while (1) {
        PORTD = Dis7seg[Value]; // Parpadea LED RA0
        __delay_ms(50);
    }
    return;
}

void __interrupt(high_priority) myHiIsr(void) {
    Value += 1;
    if (Value > 9) {
        Value = 0;
    }
    INTCONbits.TMR0IF = 0;
}

void configuracion(void) {
    TRISA = 0;
    TRISD = 0;
    ANSELA = 0;
    ANSELD = 0;
    LATA = 0;
    
    INTCON = 0xA0; // Habilita interrupcion por TMR0 
    RCONbits.IPEN = 1; // Habilita niveles de interrupci�n  
    T0CON = 0xE0; // Configuramos el pin T0CKI y el timer de 8 bits, prescaler 1:2
}