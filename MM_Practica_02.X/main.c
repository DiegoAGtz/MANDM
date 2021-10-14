/*
 * File:   main.c
 * Author: diego
 *
 * Created on 25 de septiembre de 2021, 10:44 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled) 

#include <xc.h> 
#define _XTAL_FREQ 1000000 // Frecuencia por default 

void Configuracion(void) 
{ 
    TRISD=0; 
    ANSELD=0; 

    TRISA = 0;  // Ejercicio 1
    ANSELA = 0; // Ejercicio 1
    
    TRISB = 0x02;   // Ejercicio 2
    ANSELB = 0x02;  // Ejercicio 2
    
    // ADCON0=0x39; // Original
    ADCON0 = 0x29;  // Ejercicio 2
    ADCON1=0x00; 
    // ADCON2=0x90;    // Original
    ADCON2 = 0x97;  // Ejercicio 1
} 

int Conversion(void) 
{ 
    ADCON0bits.GO = 1; 
    while (ADCON0bits.GO); 
    return ADRESL + ADRESH*256; // Retorna los 10 bits como int 
} 

void main(void) { 
    Configuracion(); 
    int conv; 
    while(1) 
    { 
        conv = Conversion();
        PORTD = conv/4;    // Despliega los 8 bits más significativos 
        PORTA = conv;      // Ejercicio 1
        PORTB = conv/8*2 | (conv/4 & 0b00000010) / 2;    // Ejercicio 2
        __delay_ms(100); 
    } 
    return; 
} 