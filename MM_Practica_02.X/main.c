/*
 * File:   main.c
 * Author: diego
 *
 * Created on 25 de septiembre de 2021, 10:44 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware (SWDTEN ignored)) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled) 

#include <xc.h> 
#define _XTAL_FREQ 1000000 // Frecuencia por default 

void configuracion(void);
int conversion(void);

void main(void) {
    configuracion();
    int conv;
    while (1) {
        conv = conversion();
        PORTB = (conv / 8 * 2) | ((conv / 4 & 0b00000010) / 2); 
        /*
            conv / 8 -> Obtenemos los 7 bits más significativos.
            * 2 -> Ahora tenemos 8 bits.
            (conv/4) -> Obtenemos los 8 bits más significativos.
            & 0b00000010 -> obtenemos si el segundo bit es 1 o 0
            / 2 -> Obtenemos 7 bits
            | -> Comparamos con OR, si el primer bit de la segunda parte es 1, se 
                 asignara al primer bit de la primera parte, en caso de ser cero,
                 el primer bit continuará igual.
            Lo asignamos al puerto B. Como el bit 2 estará ocupado en el puerto B
            no importa que valor sea pues ese pin esta configurado como entrada.
        */
        __delay_ms(100);
    }
    return;
}

void configuracion(void) {
    TRISB = 0x02; // Asignamos a todo el puerto B, excepto el bit 2, como salida
    ANSELB = 0x02; // Asignamos a todo el puerto B, excepto el bit 2, como digital

    ADCON0 = 0x29; // Asignamos el canal análogico AN10
    ADCON1 = 0x00;
    ADCON2 = 0x97; // Oscilador de 600kHz como fuente de reloj
}

int conversion(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ADRESL + ADRESH * 256; // Retorna los 10 bits como int 
} 