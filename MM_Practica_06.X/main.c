/*
 * File:   main.c
 * Author: diego
 *
 * Created on 11 de octubre de 2021, 02:50 PM
 */
// Program for generating a PWM output 
#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored))
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled) 

#include <xc.h> 
#include <pic18f45k50.h> 

#define _XTAL_FREQ 1000000 // Frecuencia por default 

void Configuracion(void);
int Conversion(void);

void main(void) {
    char Datos = 0;
    Configuracion();
    while (1) {
        CCPR1L = Conversion() / 4; // Despliega los 8 bits más significativos 
        Datos = ADRESL << 4;
        Datos = Datos & 0x30; //Enmascara todos los bits menos 5:4 
        CCP1CON = CCP1CON | Datos;
        __delay_ms(100);
    }
    return;
}

void Configuracion(void) {
    OSCCON = 0x23;
    TRISC = 0;
    ANSELC = 0;
    ADCON0 = 0x01;
    ADCON1 = 0x00;
    ADCON2 = 0x90;
    T2CON = 0x06; //Postscaler 1:1, prescaler 1:16 
    CCP1CON = 0x0C; // Selecciona modo PWM 
    CCPR1L = 0x0B; // Carga CCPR1L con un valor arbitrario 
    PR2 = 0xFF;
}

int Conversion(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ADRESL + ADRESH * 256; // Retorna los 10 bits como int 
}