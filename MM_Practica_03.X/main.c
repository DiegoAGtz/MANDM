/*
 * File:   main.c
 * Author: diego
 *
 * Created on 28 de septiembre de 2021, 02:58 PM
 */
// Manejo de 2 interrupciones externas 
#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled) 

#include <xc.h> 
#define _XTAL_FREQ 1000000 // Frecuencia por default

/* void __interrupt(high_priority) myHiIsr(void) 
{ 
    char A = 10; 
    while(A) 
    { 
        PORTD = 0xFF; 
        __delay_ms(400); 

        PORTD = 0x00; 
        __delay_ms(400); 
        A -= 1; 
    } 

    INTCONbits.INT0IF=0;
}

void __interrupt(low_priority) myLoIsr(void) 
{ 
    char A = 10; 
    while(A) 
    { 
        PORTD = A;
        __delay_ms(500); 

        A -= 1; 
    }
    INTCON3bits.INT1F=0; 
} ORIGINAL*/

void parpadeo(void) 
{ 
    char A = 10; 
    while(A) 
    { 
        PORTD = 0xFF; 
        __delay_ms(400); 

        PORTD = 0x00; 
        __delay_ms(400); 
        A -= 1; 
    } 
}

void contador(void) 
{ 
    char A = 10; 
    while(A) 
    { 
        PORTD = A;
        __delay_ms(500); 

        A -= 1; 
    }
}

void __interrupt(high_priority) prioridadAlta(void) 
{
    if(INTCONbits.INT0IF) {
        parpadeo();
        INTCONbits.INT0IF=0;
    } else {
        contador();
        INTCON3bits.INT1F=0; 
    }
}

void __interrupt(low_priority) myLoIsr(void) 
{ 
    char A = 15; 
    while(A) 
    { 
        PORTD = A;
        __delay_ms(500); 

        A -= 1; 
    }
    INTCON3bits.INT2F=0; 
}

void Configuracion(void) 
{ 
    TRISA=0; 
    TRISB=0xFF; 
    TRISD=0; 
    ANSELA=0; 
    ANSELB=0; 
    ANSELD=0; 
    LATA = 0; 
    LATD = 0; 
    INTCON=0xD0; // Habilita interrupcion externa RBO 
    // INTCON3=0x08; // Habilita interrupción externa RB1, baja prioridad  ORIGINAL
    // INTCON3 = 0x48;     // Ejercicio 1
    INTCON3 = 0x58;     // Ejercicio 2
    RCONbits.IPEN = 1; // Habilita niveles de interrupción 
} 

void main(void) { 
    Configuracion(); 
    while(1) 
    { 
        PORTA ^= 1; // Parpadea LED RA0 
        __delay_ms(500); 
    } 
    return; 
}
