/*
 * File:   main.c
 * Author: diego
 *
 * Created on 20 de septiembre de 2021, 12:47 PM
 */

#pragma config FOSC = INTOSCIO  // Se selecciona oscilador interno
#pragma config WDTEN = OFF      // Se deshabilita el perro guardian
#pragma config MCLRE = ON       // Se deshabilita el pin 1 como reset externo
#pragma config LVP = OFF        // Se deshabilita la programación por bajo voltaje
#pragma config ICPRT = OFF      // Se deshabilita el pin para programación y depuración

#include <xc.h>                 // Archivo de cabecera para todos los programas

#define _XTAL_FREQ 1000000      // Constante que indica la frecuencia a la que opera el microcontrolador

void configura(void);
void convierte(void);

void main(void) {
    configura();
    __delay_ms(1);
    
    while(1) {
        convierte();
        __delay_ms(50);    // Función de retardo en milisegundos
    }
    return;
}

void configura(void) {
    TRISA = 0;      // Registro para definir los pines del puerto A como salidas (todos como 0 -> 0, todos como 1 -> 255, 8 pines)
    ANSELA = 0b00000001;     // Registro para definir los pines del puerto A como digitales (todos como 0 y el menos significativo 1)
    
    TRISC = 0;
    ANSELC = 0;
    
    ADCON0 = 0b00000001;
    ADCON1 = 0b00000000;
    ADCON2 = 0b00010000;
    
    CCP1CON = 0x0F;
    T2CON = 0x07;
    PR2 = 155;
}

void convierte(void) {
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO);
    CCPR1L = ADRESH;
    
    if(ADRESL > 127)
        CCP1CONbits.DC1B1 = 1;
    else
        CCP1CONbits.DC1B1 = 0;
    
    ADRESL = ADRESL * 2;
    
    if(ADRESL > 127)
        CCP1CONbits.DC1B0 = 1;
    else
        CCP1CONbits.DC1B0 = 0;
}