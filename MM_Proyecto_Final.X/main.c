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
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 4000000      // Frecuencia del micro

char Bandera = 0;
char Indice = 0;
int Valor = 0;

void configuracion(void);
void __interrupt(high_priority) IAP(void);
void InicializaLCD(void);
void putch(char data);
void putcm(char data);
void limpiaLCD(void);
int mapeo(int valor, int minEntrada, int maxEntrada, int minSalida, int maxSalida);

void main(void) {
    configuracion();
    InicializaLCD();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    printf(" Curso de M&M ");
    putcm(0xC2);
    printf(" Year: %d ", 2021);
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
    TRISD = 0;
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;

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
    char Letra[3];
    Letra[Indice] = RCREG1;
    if (Letra[Indice] == '\r' || Indice == 2) {
        Indice = 0;
        LATB = atoi(Letra);
        Valor = mapeo(atoi(Letra), 0, 100, 0, 149);
        CCPR1L = Valor;
        limpiaLCD();
        putcm(0xC2);
        printf("%d%% - %d", atoi(Letra), Valor);
    } else if (Letra[Indice] < 48 || Letra[Indice] > 57) {
        // Código incorrecto
        limpiaLCD();
        putcm(0xC2);
        printf("Error.");
        Indice = 0;
        LATB = 255;
    } else {
        Indice++;
    }
    // PIR1bits.RCIF = 0;   // No es necesaria. Se limpia al leer RCREG1
}

void InicializaLCD(void) {
    __delay_ms(30);
    putcm(0x02); // Inicializa en modo 4 bits
    __delay_ms(1);

    putcm(0x28); // Inicializa en 2 lineas 5x7
    __delay_ms(1);

    putcm(0x2C);
    __delay_ms(1);

    putcm(0x0C);
    __delay_ms(1);

    putcm(0x06);
    __delay_ms(1);

    putcm(0x80); // Posiciona el cursor en 1,1
    __delay_ms(1);
}

void putch(char data) {
    char Activa;
    Activa = data & 0xF0;
    LATD = Activa | 0x05; // 0bxxxx0101
    __delay_us(10);

    LATD = Activa | 0x01; // 0bxxxx0001
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

void limpiaLCD(void) {
    putcm(0x01); //Limpiamos LCD
    putcm(0x84); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    printf("Proyecto Final");
}

int mapeo(int valor, int minEntrada, int maxEntrada, int minSalida, int maxSalida) {
    return (int) ((valor - minEntrada)*(maxSalida - minSalida) / (maxEntrada - minEntrada) + minSalida);
}