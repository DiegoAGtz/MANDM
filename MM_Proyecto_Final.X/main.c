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

#define _XTAL_FREQ 4000000      // Frecuencia del microcontrolador

char Letra[3];
char Bandera = 0;
char Indice = 0;
int Valor = 0;
char Encendido = 0;

void configuracion(void);
void __interrupt(high_priority) IAP(void);
void inicializaLCD(void);
void putch(char data);
void putcm(char data);
void limpiaLCD(void);
int mapeo(int valor, int minEntrada, int maxEntrada, int minSalida, int maxSalida);
void ejecutarComando();
char isNumber(char v);
void modificarPotencia();
void guardarEEPROM(char val);
char leerEEPROM();
void enviarPWM(int val);
void enviarPSerial(char val);
void enviarMSerial(char *err);

void main(void) {
    configuracion();
    inicializaLCD();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    char tmp = leerEEPROM();
    Valor = mapeo(tmp, 0, 100, 0, 255)*4 + mapeo(tmp, 0, 100, 0, 3);

    printf("Motor: OFF      ");
    putcm(0xC0);
    __delay_us(10);
    printf("Potencia: %d%%  ", tmp);
    enviarPSerial(tmp);
    // printf("%d%% - %d", tmp, Valor);

    LATCbits.LATC1 = 1;
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

    EEADR = 0;
}

void __interrupt(high_priority) IAP(void) {
    Letra[Indice] = RCREG1;

    if(!Bandera) {
        if((!isNumber(Letra[0]) && Indice == 2) || (!isNumber(Letra[0]) && Letra[Indice] == '\r')) {
            ejecutarComando();
        } else if ((isNumber(Letra[0]) && Indice == 2) || (isNumber(Letra[0]) && Letra[Indice] == '\r')) {
            modificarPotencia();
        } else {
            Indice++;
        }
    } 

    if (Letra[Indice] == '\r' && Bandera) {
        Indice = 0;
        Bandera = 0;
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
    putcm(0x80); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    if(Encendido) {
        printf("Motor: ON       ");
    } else {
        printf("Motor: OFF      ");
    }
    putcm(0xC0);
}

int mapeo(int valor, int minEntrada, int maxEntrada, int minSalida, int maxSalida) {
    // if (valor > maxEntrada) valor = maxEntrada;
    // else if (valor < minEntrada) valor = minEntrada;
    // return (int) ((valor - minEntrada)*(maxSalida - minSalida) / (maxEntrada - minEntrada) + minSalida);
    int multi = valor*maxSalida/maxEntrada;
    return (int) multi;
}

void ejecutarComando() {
    Bandera = 1;
    if((Letra[0] == 'O' || Letra[0] == 'o') && (Letra[1] == 'N' || Letra[1] == 'n') && Letra[2] == '\r') {
        // Enciende motor
        Encendido = 1;
        limpiaLCD();
        enviarPWM(Valor);
        enviarMSerial("Motor Encendido.");
    } else if((Letra[0] == 'O' || Letra[0] == 'o') && (Letra[1] == 'F' || Letra[1] == 'f') && (Letra[2] == 'F' || Letra[2] == 'f')) {
        // Apaga motor
        Encendido = 0;
        limpiaLCD();
        enviarPWM(0);
        enviarMSerial("Motor Apagado.");
    } else {
        // Comando erroneo
        // Error
        limpiaLCD();
        printf("Cmd. Erroneo.   ");
        enviarMSerial("Comando Erroneo.");
    }
}

char isNumber(char v) {
    if (v >= 48 && v <= 57) return 1;
    return 0;
}

void modificarPotencia() {
    // Máximo permitido o salto de linea
    Bandera = 1;
    char i=0;
    char error = 0;
    while(i<=2 && Letra[i] != '\r') {
        if(!isNumber(Letra[i])) {
            error = 1;
            break;
        }
        i++;
    }

    if(error) {
        // Potencia incorrecta
        limpiaLCD();
        printf("Cant. Erronea.  ");
        enviarMSerial("Cantidad Erronea.");
    } else {
        // Modifica la potencia
        int tmp = atoi(Letra);

        if (tmp > 100) tmp = 100;
        else if (tmp < 0) tmp = 0;

        Valor = mapeo(tmp, 0, 100, 0, 255)*4 + mapeo(tmp, 0, 100, 0, 3);
        if (Encendido) {
            enviarPWM(Valor);
        }

        guardarEEPROM(tmp);
        limpiaLCD();
        __delay_us(10);
        printf("Potencia: %d%%  ", tmp);
        enviarPSerial(tmp);
    }
}

void guardarEEPROM(char val) {
    PIE1bits.RCIE = 0;
    EEDATA = val;
    EECON1bits.WREN = 1;    // Habilitar escritura
    EECON2 = 0x55;  // Antes de mandar escribir hay que mandar estas instrucciones
    EECON2 = 0xAA;  // Ordenes del fabricante. Salto de fe.
    EECON1bits.WR = 1;  // Escribir
    __delay_ms(50);
    EECON1bits.WREN = 0;    // Desactivar escritura
    PIE1bits.RCIE = 1;
}

char leerEEPROM() {
    char value = 0;
    EECON1 = 0b00000001;
    __delay_us(10);
    value = EEDATA;
    __delay_us(50);
    return value;
}

void enviarPWM(int val) {
    int Datos;
    CCPR1L = val / 4; // Despliega los 8 bits más significativos 
    Datos = val << 4;
    Datos = Datos & 0x30; //Enmascara todos los bits menos 5:4 
    CCP1CON = CCP1CON | Datos;
}

void enviarPSerial(char val) {
    char s[256];
    sprintf(s, "Potencia actual del motor: %d%% - equivale a %d\r", val, Valor);
    int i=0;
    while(s[i] != '\0') {
        TXREG1 = s[i];
        __delay_ms(10);
        i++;
    }
}

void enviarMSerial(char *err) {
    char s[256];
    sprintf(s, "%s\r", err);
    int i=0;
    while(s[i] != '\0') {
        TXREG1 = s[i];
        __delay_ms(10);
        i++;
    }
}
