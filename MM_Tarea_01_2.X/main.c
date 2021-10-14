/*
 * File:   main.c
 * Author: diego
 *
 * Created on 4 de octubre de 2021, 02:36 PM
 */
#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator)
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored))
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled)
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>
#define _XTAL_FREQ 1000000 // Frecuencia por default
#include <stdio.h>

void InicializaLCD(void);
void Configuracion(void);
int Conversion(void);
void putch(char data);
void putcm(char data);

void main(void) {
    char direccion = 1; //1 arriba, 2 abajo 01 10
    char motor = 0;
    char enProceso = 0;
    int contador = 0;
    int potenciometro;

    Configuracion();
    InicializaLCD();
    printf("Puerta cerrada");

    while (1) {
        putcm(0xC2);
        printf("Contador: %d ", contador);
        potenciometro = Conversion();
        if (potenciometro >= 800 && motor) {
            motor = 0;
            if (direccion == 2) {
                contador++;
                direccion = 1;
            } else {
                direccion = 2;
            }
            enProceso = 0;
        } else if (potenciometro < 800 && PORTAbits.RA0) {
            if (direccion == 2 && enProceso) {
                direccion = 1;
            }
            if (motor) {
                motor = 0;
            } else {
                motor = direccion;
            }
            enProceso = 1;
            __delay_ms(500);
        }
        LATE = motor;
        __delay_ms(100);
    }
    return;
}

void Configuracion(void) {
    TRISA = 1;
    ANSELA = 0;
    TRISD = 0; //Salidas digitales
    ANSELD = 0;
    TRISE = 0;
    ANSELE = 0;
    ADCON0 = 0x39;
    ADCON1 = 0x00;
    ADCON2 = 0x90;
}

int Conversion(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ADRESL + ADRESH * 256; // Retorna los 10 bits como int
}

void putch(char data) {
    char Activa;
    Activa = data & 0xF0;
    LATD = Activa | 0x05; //0bxxxx0101
    __delay_us(10);
    LATD = Activa | 0x01; //0bxxxx0001
    __delay_ms(1);
    Activa = data << 4;
    LATD = Activa | 0x05;
    __delay_us(10);
    LATD = Activa | 0x01;
}

void putcm(char data) {
    char Activa;
    Activa = data & 0xF0; //0bxxxx0100
    LATD = Activa | 0x04;
    __delay_us(10);
    LATD = Activa;
    __delay_ms(1);
    Activa = data << 4;
    LATD = Activa | 0x04;
    __delay_us(10);
    LATD = Activa;
}

void InicializaLCD(void) {
    __delay_ms(30);
    putcm(0x02); // Inicializa en modo 4 bits
    __delay_ms(1);
    putcm(0x28); // Inicializa en 2 líneas 5x7
    __delay_ms(1);
    putcm(0x2C);
    __delay_ms(1);
    putcm(0x0C);
    __delay_ms(1);
    putcm(0x06);
    __delay_ms(1);
    putcm(0x80); //Posiciona el curson en 1,1
    __delay_ms(1);
}
