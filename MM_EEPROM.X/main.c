/*
 * File:   main.c
 * Author: diego
 *
 * Created on 13 de septiembre de 2021, 12:41 PM
 */

#pragma config FOSC = INTOSCIO
#pragma config WDTEN = OFF
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF

#include <xc.h>

#define _XTAL_FREQ 1000000

char Conta = 0;

void __interrupt(high_priority) Externa0(void) {
    Conta += 1;
    EEDATA = Conta;
    EECON1bits.WREN = 1;    // Habilitar escritura
    EECON2 = 0x55;  // Antes de mandar escribir hay que mandar estas instrucciones
    EECON2 = 0xAA;  // Ordenes del fabricante. Salto de fe.
    EECON1bits.WR = 1;  // Escribir
    __delay_ms(50);
    EECON1bits.WREN = 0;    // Desactivar escritura
    
    INTCONbits.INT0IF = 0;
}

void main(void) {
    TRISA = 0;
    ANSELA = 0;
    LATA = 0;
    
    TRISB = 1;
    ANSELB = 0;
    
    INTCON = 0b10010000;
    RCONbits.IPEN = 1;
    
    EEADR = 0;
    EECON1 = 0b00000001;
    __delay_us(10);
    Conta = EEDATA;
    
    while(1) {
        PORTBbits.RB3 ^= 1;     // Parpadea LED con RA0        
        PORTA = Conta;
        __delay_ms(500);
    }
    
    return;
}