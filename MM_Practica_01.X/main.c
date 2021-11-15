/* File:   main.c
 * Author: diego
 *
 * Created on 25 de septiembre de 2021, 09:48 PM
 */

#pragma config FOSC = INTOSCIO // Oscillator Selection (Internal oscillator) 
#pragma config WDTEN = OFF // Watchdog Timer Enable bits (WDT disabled in hardware) 
#pragma config MCLRE = ON // Master Clear Reset Pin Enable (MCLR pin enabled; RE3 input disabled) 
#pragma config LVP = OFF // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled) 
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port Enable (ICPORT disabled)

#include <xc.h> 

#define _XTAL_FREQ 1000000 // Frecuencia por default 

void main(void) {
    TRISA = 0;
    ANSELA = 0;
<<<<<<< HEAD
    LATA = 0;
=======
>>>>>>> cd28643c24261472ab76b5bd65873cef17a92772

    char contador = 0xFF; // Variable que usaremos para el contador
    while (1) {
        LATA = contador; // Asignamos contador a LATA
        if (--contador < 0) { // Disminuimos y comprobamos
            contador = 0xFF; // Regresamos a su valor inicial
        }
        __delay_ms(500);
        __delay_ms(500);    // Añadimos un delay extra
    }
    return;
}