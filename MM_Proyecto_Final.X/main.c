/*
 * File:   main.c
 * Author: Diego Armando Gutiérrez Ayala
 *
 * Created on 25 de octubre de 2021, 05:25 PM
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

char Letra[3];
char Bandera = 0;
char Indice = 0;
int Valor = 0;

void configuracion(void);
void __interrupt(high_priority) IAP(void);
void inicializaLCD(void);
void putch(char data);
void putcm(char data);
void limpiaLCD(void);
int mapeo(int valor, int minEntrada, int maxEntrada, int minSalida, int maxSalida);

void main(void) {
    configuracion();
    inicializaLCD();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    printf(" Curso de M&M ");
    putcm(0xC2);
    printf(" Year: %d ", 2021);
    while (1) {
        LATCbits.LC0 ^= 1;
        __delay_ms(500);
    }

    return;
}

void configuracion(void) {
    TRISC = 0xC0; // b?11000001?
    TRISD = 0;
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
    Letra[Indice] = RCREG1;
    
    if ((Indice == 2 && Letra[Indice] >= 48 && Letra[Indice] <= 57) || (Letra[Indice] == '\r' && Indice > 0) || Bandera) {
        // Máximo permitido o salto de linea
        if (!Bandera) {
            Bandera = 1;
            int tmp = atoi(Letra);
            Valor = mapeo(tmp, 0, 100, 0, 155);
            CCPR1L = Valor;
            limpiaLCD();
            putcm(0xC2);
            printf("%d%% - %d", tmp, Valor);
        }
        if (Letra[Indice] == '\r' && Bandera) {
            Indice = 0;
            Bandera = 0;
        }
    } else if (Letra[Indice] < 48 || Letra[Indice] > 57) {
        // Error
        Indice = 0;
        Bandera = 0;
        limpiaLCD();
        putcm(0xC2);
        printf("Error.");
    } else {
        Indice++;
    }
}

void inicializaLCD(void) {
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
    if (valor > maxEntrada) valor = maxEntrada;
    else if (valor < minEntrada) valor = minEntrada;
    return (int) ((valor - minEntrada)*(maxSalida - minSalida) / (maxEntrada - minEntrada) + minSalida);
}