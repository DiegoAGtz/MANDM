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

char Bandera = 0;
int Valor = 0;

void configuracion(void);
void __interrupt(high_priority) IAP(void);

void main(void) {
    configuracion();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    while (1) {
        LATAbits.LA7 ^= 1;
        __delay_ms(500);
    }

    return;
}

void configuracion(void) {
    TRISA = 0;
    TRISB = 0;
    TRISC = 0xC0; // b?11000000?
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;

    INTCON = 0b10000000; // Activa interrupciones de alta prioridad
    RCONbits.IPEN = 1; // Activa alta y baja prioridad
    PIE1bits.RCIE = 1;

    OSCCON = 0x53; //4MHz

    SPBRG1 = 25;
    TXSTA1 = 0x24;
    SPBRGH1 = 0x00;
    RCSTA1 = 0x90;

    CCP1CON = 0x0F;
    T2CON = 0x07;
    PR2 = 155;
    CCPR1L = 0;
}

void __interrupt(high_priority) IAP(void) {
    char Letra;
    Letra = RCREG1;
    if(Letra == 65) {
        LATAbits.LA0 ^= 1;
    } else if(Letra == 66) {
        LATAbits.LA1 ^= 1;
    } else if(Letra == 67) {
        if(Bandera == 0) {
            Valor += 30;
            CCPR1L = Valor;
            LATB = Valor;
            if(Valor > 149) {
                Bandera = 1;
            }
        } else {
            Valor -= 30;
            CCPR1L = Valor;
            LATB = Valor;
            if(Valor < 30) {
                Bandera = 0;
            }
        }
    }
    // PIR1bits.RCIF = 0;   // No es necesaria. Se limpia al leer RCREG1
}
