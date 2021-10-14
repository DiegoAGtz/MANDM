/*
 * File:   main.c
 * Author: diego
 *
 * Created on 11 de octubre de 2021, 09:47 AM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h> 
#include <pic18f45k50.h> 

#define _XTAL_FREQ 1000000 // Frecuencia por default 

char Value;

void Configuracion(void);
void __interrupt(high_priority) myHiIsr(void);
char LeeEEprom(char Direccion);
void EscribeEEprom(char Direccion, char Valor);

void main(void) {
    const char Dis7seg[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x73};
    Configuracion();
    Value = LeeEEprom(0);

    while (1) {
        PORTD = Dis7seg[Value]; // Parpadea LED RA0 
        __delay_ms(50);

    }
    return;
}

void Configuracion(void) {
    TRISA = 0; // Ejercicio 4
    ANSELA = 0;
    LATA = 0;
    
    TRISD = 0;
    ANSELD = 0;
    LATD = 0;
    INTCON = 0xA0; // Habilita interrupcion por TMR0 
    RCONbits.IPEN = 1; // Habilita niveles de interrupción 
    T0CON = 0x83; // Habilita TMR0 con razón 1:16 
    PIE2bits.EEIE = 1;  // Ejercicio 3

    EECON1bits.EEPGD = 0; //Selecciona memoria de datos 
    EECON1bits.CFGS = 0; // Selecciona memoria EEPROM 
}

void __interrupt(high_priority) myHiIsr(void) {
    if (INTCONbits.TMR0IF) {
        Value += 1;

        if (Value > 9) {
            Value = 0;
        }
        EscribeEEprom(0, Value);
        INTCONbits.TMR0IF = 0;
    } else if (PIR2bits.EEIF) {     // Ejercicio 4
        LATA ^= 1;
        PIR2bits.EEIF = 0;
    }
}

char LeeEEprom(char Direccion) {
    EEADR = Direccion;
    EECON1bits.RD = 1;
    while (EECON1bits.RD); //Espera a que termine de escibir 
    return EEDATA;
}

void EscribeEEprom(char Direccion, char Valor) {
    EEADR = Direccion;
    EEDATA = Valor;

    EECON1bits.WREN = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while (EECON1bits.WR);

    EECON1bits.WREN = 0;
}