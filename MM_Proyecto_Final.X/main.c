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
#include <stdio.h>      // Funciones necesarias para mostrar mensajes en la LCD.
#include <stdlib.h>     // Función necesaria para convertir de cadena a entero.

#define _XTAL_FREQ 4000000      // Frecuencia del microcontrolador

char entrada_serial[3];
char indice_es = 0;
char bandera = 0;
char motor_encendido = 0;
char es_erronea = 0;
int potencia_pwm = 0;

void configuracion(void);
void __interrupt(high_priority) IAP(void);

// Funciones encargadas de la pantalla LCD.
void inicializa_lcd(void);
void putch(char data);
void putcm(char data);
void limpia_lcd(void);
void escribe_potencia_lcd(char potencia);
void escribe_error_lcd(char *error);

// Convierte el valor recibido a otro en un rango distinto.
int mapeo(int valor, int min_entrada, int max_entrada, int min_salida, int max_salida);
// Verifica si el caracter recibido es un dígito.
char es_digito(char v);

// Ejecuta el comando recibido desde el puerto serial.
void ejecutar_comando();
// Modifica la potencia con la que esta trabajando el motor.
void modificar_potencia();
// Cambia los valores de CCPR1L:CCP1CON<5:4> con el valor recibido.
void enviar_pwm(int val);

// Obtiene el valor de la memoria EEPROM, posición 0.
char leer_eeprom();
// Guarda el dato en la memoria EEPROM, posición 0.
void guardar_eeprom(char val);

// Envia por el puerto serial un mensaje con la potencia a la cual esta trabajando el motor.
void enviar_potencia_serial(char val);
// Envia por el puerto serial un mensaje.
void enviar_mensaje_serial(char *err);

void main(void) {
    configuracion();
    inicializa_lcd();
    __delay_ms(10); // Tiempo de asentamiento para el módulo    

    char tmp = leer_eeprom();
    potencia_pwm = mapeo(tmp, 0, 100, 0, 255)*4 + mapeo(tmp, 0, 100, 0, 3);

    escribe_potencia_lcd(tmp);
    escribe_error_lcd("Motor: OFF");
    enviar_potencia_serial(tmp);

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

    INTCON = 0x80; // Activa interrupciones de alta prioridad
    RCONbits.IPEN = 1; // Activa alta y baja prioridad
    PIE1bits.RCIE = 1;

    OSCCON = 0x53; //4MHz

    SPBRG1 = 25;
    TXSTA1 = 0x24;
    SPBRGH1 = 0x00;
    RCSTA1 = 0x90;

    CCP1CON = 0x0C;
    T2CON = 0x07;
    PR2 = 0xFF;
    CCPR1L = 0;

    EEADR = 0;
}

void __interrupt(high_priority) IAP(void) {
    entrada_serial[indice_es] = RCREG1;

    if(!bandera) {
        if((!es_digito(entrada_serial[0]) && indice_es == 2) || (!es_digito(entrada_serial[0]) && entrada_serial[indice_es] == '\r')) {
            ejecutar_comando();
        } else if ((es_digito(entrada_serial[0]) && indice_es == 2) || (es_digito(entrada_serial[0]) && entrada_serial[indice_es] == '\r')) {
            modificar_potencia();
        } else {
            indice_es++;
        }
    } 

    if (entrada_serial[indice_es] == '\r' && bandera) {
        indice_es = 0;
        bandera = 0;
    }
}

void inicializa_lcd(void) {
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

void limpia_lcd(void) {
    putcm(0x80); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    //printf("Potencia: %d%%  ", tmp);
    putcm(0xC0);
}

void escribe_potencia_lcd(char potencia) {
    putcm(0x80); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    __delay_us(10);
    printf("Potencia: %d%%  ", potencia);
}

void escribe_error_lcd(char *error) {
    putcm(0x80);
    __delay_us(10);
    putcm(0xC0);
    __delay_us(10);
    printf("%s              ", error);
}

int mapeo(int valor, int min_entrada, int max_entrada, int min_salida, int max_salida) {
    if (valor > max_entrada) valor = max_entrada;
    else if (valor < min_entrada) valor = min_entrada;
    return (int) ((valor - min_entrada)*(max_salida - min_salida) / (max_entrada - min_entrada) + min_salida);
}

void ejecutar_comando() {
    bandera = 1;
    if((entrada_serial[0] == 'O' || entrada_serial[0] == 'o') && (entrada_serial[1] == 'N' || entrada_serial[1] == 'n') && entrada_serial[2] == '\r') {
        // Enciende motor
        motor_encendido = 1;
        enviar_pwm(potencia_pwm);
        escribe_error_lcd("Motor: ON");
        enviar_mensaje_serial("Motor motor_encendido.");
    } else if((entrada_serial[0] == 'O' || entrada_serial[0] == 'o') && (entrada_serial[1] == 'F' || entrada_serial[1] == 'f') && (entrada_serial[2] == 'F' || entrada_serial[2] == 'f')) {
        // Apaga motor
        motor_encendido = 0;
        enviar_pwm(0);
        escribe_error_lcd("Motor: OFF");
        enviar_mensaje_serial("Motor Apagado.");
    } else {
        // Comando erroneo
        escribe_error_lcd("Cmd. Erroneo.");
        enviar_mensaje_serial("Comando Erroneo.");
        __delay_ms(1000);
        if(motor_encendido) {
            escribe_error_lcd("Motor: ON");
        } else {
            escribe_error_lcd("Motor: OFF");
        }
    }
}

char es_digito(char v) {
    if (v >= 48 && v <= 57) return 1;
    return 0;
}

void modificar_potencia() {
    // Máximo permitido o salto de linea
    bandera = 1;
    char i=0;
    char error = 0;
    while(i<=2 && entrada_serial[i] != '\r') {
        if(!es_digito(entrada_serial[i])) {
            error = 1;
            break;
        }
        i++;
    }

    if(error) {
        // Potencia incorrecta
        escribe_error_lcd("Cant. Erronea.");
        enviar_mensaje_serial("Cantidad Erronea.");
        __delay_ms(1000);
        if(motor_encendido) {
            escribe_error_lcd("Motor: ON");
        } else {
            escribe_error_lcd("Motor: OFF");
        }
    } else {
        // Modifica la potencia
        int tmp = atoi(entrada_serial);

        if (tmp > 100) tmp = 100;
        else if (tmp < 0) tmp = 0;

        potencia_pwm = mapeo(tmp, 0, 100, 0, 255)*4 + mapeo(tmp, 0, 100, 0, 3);
        if (motor_encendido) {
            enviar_pwm(potencia_pwm);
        }

        guardar_eeprom(tmp);
        escribe_potencia_lcd(tmp);
        enviar_potencia_serial(tmp);
    }
}

void guardar_eeprom(char val) {
    INTCONbits.GIE = 0; // Desactiva interrupciones
    EEDATA = val;
    EECON1bits.WREN = 1;    // Habilitar escritura
    EECON2 = 0x55;  // Antes de mandar escribir hay que mandar estas instrucciones
    EECON2 = 0xAA;  // Ordenes del fabricante. Salto de fe.
    EECON1bits.WR = 1;  // Escribir
    __delay_ms(50);
    EECON1bits.WREN = 0;    // Desactivar escritura
    INTCONbits.GIE = 1; // Habilita interrupciones
}

char leer_eeprom() {
    char value = 0;
    EECON1 = 0b00000001;
    __delay_us(10);
    value = EEDATA;
    __delay_us(50);
    return value;
}

void enviar_pwm(int val) {
    int Datos;
    CCPR1L = val / 4; // Despliega los 8 bits más significativos 
    Datos = val << 4;
    Datos = Datos & 0x30; //Enmascara todos los bits menos 5:4 
    CCP1CON = CCP1CON | Datos;
}

void enviar_potencia_serial(char val) {
    char s[256];
    sprintf(s, "Potencia actual del motor: %d%% - equivale a %d.", val, potencia_pwm);
    enviar_mensaje_serial(s);
}

void enviar_mensaje_serial(char *err) {
    char s[256];
    sprintf(s, "%s\r", err);
    int i=0;
    while(s[i] != '\0') {
        TXREG1 = s[i];
        __delay_ms(10);
        i++;
    }
}
