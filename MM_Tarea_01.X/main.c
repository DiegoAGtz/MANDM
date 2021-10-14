/*
 * File:   main.c
 * Author: Angel Mauricio Per
 *
 * Created on 1 de octubre de 2021, 12:10 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>
#include <stdio.h>
#define _XTAL_FREQ 1000000 // Frecuencia por default 

char direccion = 1; // Hacia donde va el motor: 1 -> Abre, 2 -> Cierra.
char enProceso = 0; // ¿Está en proceso de abrir o cerrar?, 1 -> Si, 2 -> No.

int contador = 998; // Número de veces que ha cerrado
int corriente; // Control de corriente

void __interrupt(high_priority) presionaBoton(void); // ¿Se ha presionado el botón?
void configuracion(void); // Configura los puertos
void conversion(void); // Convertidor análogico dígital
void cerrar(void); // ¿La corriente es mayor a 800?
void putch(char data);
void putcm(char data);
void inicializaLCD(void);
void limpiaLCD(void);

void main(void) {
    configuracion();
    __delay_ms(10);
    inicializaLCD();

    printf("Total de cierres");
    putcm(0xC2);
    printf("     %d     ", contador);

    while (1) {
        conversion();
        cerrar();
        __delay_ms(100);
    }
    return;
}

void __interrupt(high_priority) presionaBoton(void) {
    if (corriente < 800) {
        if (direccion == 2 && enProceso) {
            direccion = 1;
            LATE = 0;
            __delay_ms(10);
            LATE = direccion;
        } else if (PORTE) {
            LATE = 0;
        } else {
            LATE = direccion;
        }
        enProceso = 1;
    }
    INTCONbits.INT0IF = 0;
}

void configuracion(void) {
    TRISB = 0xFF;
    ANSELB = 0;

    TRISD = 0;
    ANSELD = 0;

    TRISE = 0;
    ANSELE = 0;

    ADCON0 = 0x39;
    ADCON1 = 0x00;
    ADCON2 = 0x90;

    INTCON = 0xD0; // Habilita interrupcion externa RBO 
    RCONbits.IPEN = 1; // Habilita niveles de interrupción
}

void conversion(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    corriente = ADRESL + ADRESH * 256; // Retorna los 10 bits como int 
}

void cerrar(void) {
    if (corriente >= 800 && PORTE) {
        enProceso = 0;
        if (direccion == 2) {
            if (contador < 999) {
                contador++;
            } else {
                contador = 0;
                limpiaLCD();
            }
            putcm(0xC2);
            printf("     %d", contador);
            direccion = 1;
        } else {
            direccion = 2;
        }
        LATE = 0;
    }
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

void inicializaLCD(void) {
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

void limpiaLCD(void) {
    putcm(0x01); //Limpiamos LCD
    putcm(0x84); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    printf("Total de cierres");
}