/*
 * File:   main.c
 * Author: diego
 *
 * Created on 8 de octubre de 2021, 06:40 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>

#define _XTAL_FREQ 1000000 // Frecuencia por default 

char serie = 0;
char indice = 0;

void configuracion(void);
void __interrupt(high_priority) presionaBoton(void);
void incrementarIndice(char limite);

void main(void) {
    char secuencia0[] = {0x55, 0xAA};
    char secuencia1[] = {0x33, 0x66, 0xCC, 0x99};
    char secuencia2[] = {0xFF, 0x00};
    configuracion();
    while(1) {
        switch(serie) {
            case 0:
                // De forma alterna
                //  0101 0101 = 0x55 85
                //  1010 1010 = 0xAA 170
                LATA = secuencia0[indice];
                LATD = secuencia0[indice];
                incrementarIndice(1);
                break;
            case 1:
                // De dos en dos
                /*
                 *  0011 0011 = 0x33 51
                 *  0110 0110 = 0x66 102
                 *  1100 1100 = 0xCC 204
                 *  1001 1001 = 0x99 153
                 */
                LATA = secuencia1[indice];
                LATD = secuencia1[indice];
                incrementarIndice(3);
                break;
            case 2:
                // Todos encendidos y apagados
                //  1111 1111 = 0xFF 255
                //  0000 0000 = 0x00 0
                LATA = secuencia2[indice];
                LATD = secuencia2[indice];                
                incrementarIndice(1);
                break;
            case 3:
                // Todos apagados
                // 0000 0000 0x00 0
                LATA = 0x00;
                LATD = 0x00;
                break;
        }
        __delay_ms(500);
    }
    return;
}

void configuracion(void) {
    TRISA = 0;
    ANSELA = 0;
    
    TRISB = 0x01;
    ANSELB = 0;
    
    TRISD = 0;
    ANSELD = 0;
    
    INTCON = 0xD0;
    RCONbits.IPEN = 1;
}

void __interrupt(high_priority) presionaBoton(void) {
    if(++serie > 3) {
        serie = 0;
    }
    indice = 0;
    INTCONbits.INT0IF = 0;
}

void incrementarIndice(char limite) {
    if(++indice > limite) {
        indice = 0;
    }
}