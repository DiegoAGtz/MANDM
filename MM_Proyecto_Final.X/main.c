/*
 * File:   main.c
 * Author: diego
 *
 * Created on 18 de octubre de 2021, 10:03 PM
 * 
 * Programa que controla el movimiento de un motor mediante PWM a través de la
 * entrada de datos por el puerto serial mediante el módulo Bluetooth.
 */

#pragma config FOSC = INTOSCIO
#pragma config WDTEN = OFF
#pragma config MCLRE = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF

#include <xc.h>

#define _XTAL_FREQ 1000000 // Frecuencia por default 

void configuracion(void);
int conversion(void);
int mapeo(int valor, float minEntrada, float maxEntrada, float minSalida, float maxSalida);

void main(void) {
    char Datos = 0;
    configuracion();
    while (1) {
        CCPR1L = conversion() / 4; // Despliega los 8 bits más significativos 
        Datos = ADRESL << 4;
        Datos = Datos & 0x30; //Enmascara todos los bits menos 5:4 
        CCP1CON = CCP1CON | Datos;
        __delay_ms(100);
    }
    return;
}

void configuracion(void) {
    // OSCCON = 0x23;
    OSCCON = 0x20; // Selecciona reloj determinado por la configuración FOSC
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

int conversion(void) {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ADRESL + ADRESH * 256; // Retorna los 10 bits como int 
}

int mapeo(int valor, float minEntrada, float maxEntrada, float minSalida, float maxSalida) {
    return (int)(valor - minEntrada)*(maxSalida - minSalida) / (maxEntrada - minEntrada) + minSalida;
}