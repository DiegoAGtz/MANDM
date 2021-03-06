/* 
 * File:   main.c
 * Author: Diego Armando Guti?rrez Ayala
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

#define _XTAL_FREQ 4000000      // Frecuencia del microcontrolador

char entrada_serial[3];
int conversion_es = 0;
char indice_es = 0;
char motor_encendido = 0;
int potencia_pwm = 0;

void configuracion(void);
void __interrupt(high_priority) iap(void);

// Funciones encargadas de la pantalla LCD.
void inicializa_lcd(void);
void putch(char data);
void putcm(char data);
void muestra_lcd(char error);

// Convierte el valor recibido a otro en un rango distinto.
int mapeo(int valor, int min_entrada, int max_entrada, int min_salida, int max_salida);
// Convierte la cadena a entero
int cadena_entero(char *cadena);
// Verifica si el caracter recibido es un d?gito.
char es_digito(char c);

// Ejecuta el comando recibido desde el puerto serial.
void ejecutar_comando(void);
// Modifica la potencia con la que esta trabajando el motor.
void modificar_potencia(void);
// Cambia los valores de CCPR1L:CCP1CON<5:4> con el valor recibido.
void enviar_pwm(int valor);

// Obtiene el valor de la memoria EEPROM en la direcci?n indicada
char leer_eeprom(char direccion);
// Guarda el dato en la memoria EEPROM en la direcci?n indicada
void guardar_eeprom(char direccion, char valor);
// Envia por el puerto serial un mensaje con la potencia a la cual esta trabajando el motor.
void enviar_potencia_serial();
// Envia por el puerto serial un mensaje.
void enviar_mensaje_serial(char *error);

void main(void) {
    configuracion();
    inicializa_lcd();
    __delay_ms(10); // Tiempo de asentamiento para el m?dulo    

    conversion_es = leer_eeprom(0);
    if (conversion_es > 100) conversion_es = 100;
    else if (conversion_es < 0) conversion_es = 0;

    potencia_pwm = mapeo(conversion_es, 0, 100, 0, 255)*4 + mapeo(conversion_es, 0, 100, 0, 3);

    LATCbits.LC0 = 0;
    enviar_pwm(0);
    muestra_lcd(0);
    enviar_potencia_serial();

    while (1);
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

    OSCCON = 0x53; // 4MHz

    SPBRG1 = 25;
    TXSTA1 = 0x24;
    SPBRGH1 = 0x00;
    RCSTA1 = 0x90;

    CCP1CON = 0x0C;
    T2CON = 0x07;
    PR2 = 0xFF;
    CCPR1L = 0;

    EECON1bits.EEPGD = 0; //Selecciona memoria de datos 
    EECON1bits.CFGS = 0; // Selecciona memoria EEPROM 
}

void __interrupt(high_priority) iap(void) {
    char caracter_serial = RCREG1;
    if (caracter_serial != '\n') {
        if (indice_es <= 2) {
            entrada_serial[indice_es] = caracter_serial;
            indice_es++;
        }
    } else {
        if ((!es_digito(entrada_serial[0]))) {
            ejecutar_comando();
        } else if ((es_digito(entrada_serial[0]))) {
            modificar_potencia();
        }
        indice_es = 0;
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

void muestra_lcd(char error) {
    putcm(0x80); //Ponemos el cursor en la posici?n inicial 0,0 del LCD
    __delay_us(10);
    printf("Potencia: %d%%  ", conversion_es);
    __delay_us(10);
    putcm(0xC0);
    __delay_us(10);
    if (error) {
        printf("Cmd/Cant Erronea");
    } else {
        if (motor_encendido) {
            printf("Motor: ON       ");
        } else {
            printf("Motor: OFF      ");
        }
    }
    __delay_us(10);
}

int mapeo(int valor, int min_entrada, int max_entrada, int min_salida, int max_salida) {
    return (int) ((valor - min_entrada)*(max_salida - min_salida) / (max_entrada - min_entrada) + min_salida);
}

int cadena_entero(char *cadena) {
    int i = 0;
    int res = 0;
    while (i <= 2 && cadena[i] != '\r') {
        res *= 10;
        res += cadena[i] - 48;
        i++;
    }
    return res;
}

char es_digito(char c) {
    if (c >= 48 && c <= 57) return 1;
    return 0;
}

void ejecutar_comando(void) {
    if ((entrada_serial[0] == 'O' || entrada_serial[0] == 'o') && (entrada_serial[1] == 'N' || entrada_serial[1] == 'n') && entrada_serial[2] == '\r') {
        motor_encendido = 1;
        enviar_pwm(potencia_pwm);
        LATCbits.LC0 = 1;
        muestra_lcd(0);
        enviar_mensaje_serial("Motor Encendido.");
    } else if ((entrada_serial[0] == 'O' || entrada_serial[0] == 'o') && (entrada_serial[1] == 'F' || entrada_serial[1] == 'f') && (entrada_serial[2] == 'F' || entrada_serial[2] == 'f')) {
        motor_encendido = 0;
        enviar_pwm(0);
        LATCbits.LC0 = 0;
        muestra_lcd(0);
        enviar_mensaje_serial("Motor Apagado.");
    } else {
        // Comando erroneo
        muestra_lcd(1);
        enviar_mensaje_serial("Comando Erroneo.");
        __delay_ms(1000);
        muestra_lcd(0);
    }
}

void modificar_potencia(void) {
    char i = 0;
    char error = 0;
    while (i <= 2 && entrada_serial[i] != '\r') {
        if (!es_digito(entrada_serial[i])) {
            error = 1;
            break;
        }
        i++;
    }

    if (error) {
        muestra_lcd(1);
        enviar_mensaje_serial("Cantidad Erronea.");
        __delay_ms(1000);
        muestra_lcd(0);
    } else {
        conversion_es = cadena_entero(entrada_serial);

        if (conversion_es > 100) conversion_es = 100;
        else if (conversion_es < 0) conversion_es = 0;

        potencia_pwm = mapeo(conversion_es, 0, 100, 0, 255)*4 + mapeo(conversion_es, 0, 100, 0, 3);
        if (motor_encendido) {
            enviar_pwm(potencia_pwm);
        }

        guardar_eeprom(0, conversion_es);
        muestra_lcd(0);
        enviar_potencia_serial();
    }
}

void enviar_pwm(int valor) {
    int Datos;
    CCPR1L = valor / 4; // Despliega los 8 bits m?s significativos 
    Datos = valor << 4;
    Datos = Datos & 0x30; //Enmascara todos los bits menos 5:4 
    CCP1CON = CCP1CON | Datos;
}

char leer_eeprom(char direccion) {
    EEADR = direccion;
    EECON1bits.RD = 1;
    while (EECON1bits.RD); //Espera a que termine de escibir 
    return EEDATA;
}

void guardar_eeprom(char direccion, char valor) {
    INTCONbits.GIE = 0; // Desactiva interrupciones
    EEADR = direccion;
    EEDATA = valor;

    EECON1bits.WREN = 1; // Habilitar escritura
    EECON2 = 0x55; // Antes de mandar escribir hay que mandar estas instrucciones
    EECON2 = 0xAA; // Ordenes del fabricante. Salto de fe.
    EECON1bits.WR = 1; // Escribir
    while (EECON1bits.WR);

    EECON1bits.WREN = 0; // Desactivar escritura
    INTCONbits.GIE = 1; // Habilita interrupciones
}

void enviar_potencia_serial() {
    char s[100];
    sprintf(s, "Potencia actual del motor: %d%%\nValor cargado en PWM (10b): %d", conversion_es, potencia_pwm);
    enviar_mensaje_serial(s);
}

void enviar_mensaje_serial(char *error) {
    char s[100];
    sprintf(s, "%s\r\n", error);
    int i = 0;
    while (s[i] != '\0') {
        TXREG1 = s[i];
        __delay_ms(10);
        i++;
    }
}
