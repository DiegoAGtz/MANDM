/*
 * File:   msin.c
 * Author: diego
 *
 * Created on 28 de septiembre de 2021, 04:25 PM
 */

// Program using TMR0 timer and a lookup table for operating a 7-segment display 
#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h>
#include <pic18f45k50.h> 

<<<<<<< HEAD
#define _XTAL_FREQ 1000000

char Value;

=======
#define _XTAL_FREQ 1000000  // Frecuencia por defaul

char Value;
>>>>>>> cd28643c24261472ab76b5bd65873cef17a92772
void __interrupt(high_priority) myHiIsr(void);
void configuracion(void);

void main(void) {
    const char Dis7seg[10] = {0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x73};
    configuracion();

    while (1) {
<<<<<<< HEAD
        PORTD = Dis7seg[Value]; // Parpadea LED RA0 
=======
        PORTD = Dis7seg[Value]; // Parpadea LED RA0
>>>>>>> cd28643c24261472ab76b5bd65873cef17a92772
        __delay_ms(50);
    }
    return;
}

void __interrupt(high_priority) myHiIsr(void) {
    Value += 1;
    if (Value > 9) {
        Value = 0;
    }
<<<<<<< HEAD
    TMR1H = 3037 >> 8;
    TMR1L = 62499;
    PIR1bits.TMR1IF = 0;
=======
    INTCONbits.TMR0IF = 0;
>>>>>>> cd28643c24261472ab76b5bd65873cef17a92772
}

void configuracion(void) {
    TRISA = 0;
    TRISD = 0;
    ANSELA = 0;
    ANSELD = 0;
    LATA = 0;
<<<<<<< HEAD
    INTCON = 0x80; // Ejercicio Tarea
    RCONbits.IPEN = 1; // Habilita niveles de interrupción
    T1CON = 0x21; // Usamos Fosc, un preescaler de 4 y leemos y escribimos usando
    // dos registros de 8bits.
    PIE1 = 0x01; // Habilitamos la interrupción para TMR1
    
    /*
     * Temporizador = (4/Fosc) * Preescaler * (x+1)
     * 1seg = (4/1000000)*4*(x+1)
     * 1/1.6e^-5 = x+1
     * x = 62500 - 1
     * x = 62499
     * 
     * Ahora tenemos que restar el valor máximo del TMR0 de 16bits
     * 2^16 = 65536
     * 
     * 65536 - 62499 = 3037
     * Este es el valor que tenemos que cargar en los bytes del TMR1
     * para generar una interrupción cada 1seg, aproximadamente.
     */
    
    TMR1H = 3037 >> 8; // Bits más significativos
    TMR1L = 62499; // Bits menos significativos
=======
    
    INTCON = 0xA0; // Habilita interrupcion por TMR0 
    RCONbits.IPEN = 1; // Habilita niveles de interrupción  
    T0CON = 0xE0; // Configuramos el pin T0CKI y el timer de 8 bits, prescaler 1:2
>>>>>>> cd28643c24261472ab76b5bd65873cef17a92772
}