/*
 * File:   interruptions.c
 * Author: diego
 *
 * Created on 6 de septiembre de 2021, 12:07 PM
 */

#pragma config FOSC = INTOSCIO
#pragma config WDTEN = OFF
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF

#include <xc.h>

#define _XTAL_FREQ 1000000

void __interrupt(high_priority) Externa0(void) {
    char Conta = 15;
    while(Conta) {
        LATA = 255;
        __delay_ms(500);
        LATA = 0;
        __delay_ms(500);
        Conta -= 1;
    }
    INTCONbits.INT0IF = 0;
}

void main(void) {
    TRISA = 0;
    ANSELA = 0;
    LATA = 0;
    
    TRISB = 255;
    ANSELB = 0;
    
    INTCON = 0b10010000;
    RCONbits.IPEN = 1;
    
    while(1) {
        PORTAbits.RA0 ^= 1;     // Parpadea LED con RA0        
        __delay_ms(500);
    }
    
    return;
}