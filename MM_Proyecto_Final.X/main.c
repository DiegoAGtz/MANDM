/*
 * File:   main.c
 * Author: diego
 *
 * Created on 23 de septiembre de 2021, 12:24 PM
 */

#pragma config FOSC = INTOSCIO
#pragma config WDTEN = OFF
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF

#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 1000000

void InicializaLCD(void);
void Configuracion(void);
void putch(char data);
void putcm(char data);

void main(void) {
    int Dato = 2021;
    Configuracion();
    InicializaLCD();
    
    printf(" Curso de M&M ");
    putcm(0xC2);
    printf(" Year: %d ", Dato);
    while(1) {
        LATAbits.LA1 ^= 1;
        __delay_ms(500);
    }
    return;
}

void Configuracion(void) {
    TRISA = 0;  // Salidas digitales
    TRISD = 0;  // Salidas digitales
    ANSELD = 0;
    ANSELA = 0;
    LATA = 0;
}

void InicializaLCD(void) {
    __delay_ms(30);
    putcm(0x02);    // Inicializa en modo 4 bits
    __delay_ms(1);
    
    putcm(0x28);     // Inicializa en 2 lineas 5x7
    __delay_ms(1);
    
    putcm(0x2C);
    __delay_ms(1);
    
    putcm(0x0C);    
    __delay_ms(1);
    
    putcm(0x06);
    __delay_ms(1);    
    
    putcm(0x80);    // Posiciona el cursor en 1,1
    __delay_ms(1);
}

void putch(char data) {
    char Activa;
    Activa = data & 0xF0;
    LATD = Activa | 0x05;   // 0bxxxx0101
    __delay_us(10);
    
    LATD = Activa | 0x01;   // 0bxxxx0001
    __delay_ms(1);
    
    Activa = data << 4;
    LATD = Activa | 0x05;
    __delay_us(10);
    LATD = Activa | 0x01;
}

void putcm(char data) {
    char Activa;
    Activa = data & 0xF0;
    LATD = Activa | 0x04;
    __delay_us(10);
    
    LATD = Activa;
    __delay_ms(1);
    Activa = data << 4;
    
    LATD = Activa | 0x04;
    __delay_us(10);
    LATD = Activa;
}
