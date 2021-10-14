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

// #define _XTAL_FREQ 1000000  // Frecuencia por defaul ORIGINAL
#define _XTAL_FREQ 4000000  // 4MHz

char Value; 
void __interrupt(high_priority) myHiIsr(void) 
{ 
    Value += 1;
    if (Value > 9) 
    { 
        Value = 0; 
    } 
    INTCONbits.TMR0IF=0;
    PIR1bits.TMR1IF = 0;
}

void Configuracion(void) 
{ 
    TRISA=0xFF; 
    TRISD=0; 
    ANSELA=0; 
    ANSELD=0; 
    // LATA = 0; 
    //INTCON=0xA0; // Habilita interrupcion por TMR0 
    INTCON = 0x80;  // Ejercicio Tarea
    RCONbits.IPEN = 1; // Habilita niveles de interrupción 
    //T0CON=0x83; // Habilita TMR0 con razón 1:16 ORIGINAL
    //T0CON = 0x81;   // Habilita TMR0 con razón 1:4, 1.2 seg
    T0CON = 0xE0;   // Ejercicio 1
    T1CON = 0x21;   // Ejercicio de Tarea   FOSC/4 y 1:4
    PIE1 = 0x01;    // Ejercicio de Tarea
} 

void main(void) { 
    const char Dis7seg[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x73}; 
    Configuracion(); 

    while(1) 
    { 
        PORTD = Dis7seg[Value]; // Parpadea LED RA0 
        __delay_ms(50); 
    } 
    return; 
} 
