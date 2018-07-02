/*
 * CONTROL PARA PLAFON CON TIRA LED RGB + LUZ PRINCIPAL
 * 
 * Caracteristicas:
 * - Encendido y apagado individual de cada color
 * - Encendido y apagado individual de la luz principal
 * - Multiples funciones especiales de secuencias de luces
 * - Regulacion de velocidad para funciones especiales
 * - Apagado general del sistema
 * 
 * File:   main.c
 * Author: gemu
 *
 * Created on 30 de junio de 2018, 15:58
 */

// PIC12F629 Configuration Bit Settings
// CONFIG
#pragma config FOSC = INTRCIO   // Oscillator Selection bits (INTOSC oscillator: I/O function on GP4/OSC2/CLKOUT pin, I/O function on GP5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)


#include <xc.h>
#include "nec_protocol.h"

#define _XTAL_FREQ          4000000


// output shadow para evitar problemas rmw
union {
    unsigned char port;
    struct {
        unsigned LEDB                   :1;
        unsigned LEDG                   :1;
        unsigned LEDR                   :1;
        unsigned GP3                    :1;
        unsigned LIGHT                  :1;
        unsigned GP5                    :1;
    };
    struct {
        unsigned RGB                    :3;
    };
} SGPIO;

/**
 * Captura de comando NEC por interrupcion del pin IRSENSOR
 */
unsigned char nec_cmd;
void __interrupt() ir_intr( void ) {
    if (INTCONbits.GPIF) {
        if(nec_cmd == NEC_NO_COMMAND) {
            nec_cmd = nec_decode();
        }
        
        SGPIO.port = GPIO; // lectura obligatoria para cancelar la interrupcion
        INTCONbits.GPIF = 0; // limpia flag para recibir otra interrupcion
    }
}

void setup() {
    TRISIO = 0b00001000; // GP3 input, el resto output
    IOC = 0b00001000; // GP3 atiende interrupciones
    SGPIO.port = 0; // todas las salidas a 0
    GPIO = SGPIO.port; 
    
    INTCONbits.GPIE = 1;	// external interrupt on GPIO3 pin(4) is enabled
    INTCONbits.GPIF = 0;	// clear the external interrrupt flag
    INTCONbits.PEIE = 1;    // peripheral intrrupt enable
    INTCONbits.GIE = 1;     // GLOBL interrupt enable
    
    OPTION_REGbits.T0CS = 0;    // usar clock interno (CLKOUT)
    OPTION_REGbits.PSA = 0;     // usar prescaler 
    OPTION_REGbits.PS = NEC_PRESC_TMR0;
}

// conmuta estados de luz principal
char light_state = 0;
void process_light() {
    light_state = light_state < 3 ? ++light_state : 0;
    
    // apago luz
    SGPIO.LIGHT = 0;
    GPIO = SGPIO.port;
    __delay_ms(10);
    
    if(light_state != 0) {
        // enciendo luz
        SGPIO.LIGHT = 1;
        GPIO = SGPIO.port;
    }
}

// conmuta estados de luces rgb
char rgb_state = 0;
void process_rgb() {
    rgb_state = rgb_state < 4 ? ++rgb_state : 0;
    
    // apago rgb
    SGPIO.LEDR = 0;
    SGPIO.LEDG = 0;
    SGPIO.LEDB = 0;
            
    switch(rgb_state) {
        case 1:
            SGPIO.LEDR = 1; // enciendo r
            break;
        case 2:
            SGPIO.LEDG = 1; // enciendo g
            break;
        case 3: 
            SGPIO.LEDB = 1; // enciendo b
            break;
        case 4:
            // enciendo rgb
            SGPIO.LEDR = 1;
            SGPIO.LEDG = 1;
            SGPIO.LEDB = 1;
            break;
    }
    
    GPIO = SGPIO.port;
}

// conmuta modos especiales
char special_state = 0;
void process_special() {
    light_state = 0;
    rgb_state = 0;
    
    SGPIO.port = 0x00; // apago todo
    GPIO = SGPIO.port;
    
    special_state = special_state < 7 ? ++special_state : 0;
}

char special_delay_state = 5;
void special_delay() {
    switch(special_delay_state) {
        case 0:
            __delay_ms(20);
            break;
        case 1:
            __delay_ms(25);
            break;
        case 2:
            __delay_ms(50);
            break;
        case 3:
            __delay_ms(100);
            break;
        case 4:
            __delay_ms(200);
            break;
        case 5:
            __delay_ms(300);
            break;
        case 6:
            __delay_ms(400);
            break;
        case 7:
            __delay_ms(500);
            break;
        case 8:
            __delay_ms(800);
            break;
        case 9:
            __delay_ms(1000);
            break;
    }
}

// apaga todas las luces
void process_off() {
    light_state = 0;
    rgb_state = 0;
    special_state = 0;
    special_delay_state = 0;
    
    SGPIO.port = 0x00; // apago todo
    GPIO = SGPIO.port;
}

// procesa modos dinamicos
void process_dinamic_modes() {
            
    switch(special_state) {
        case 1: // blink rgb
            // enciendo rgb
            SGPIO.RGB = 0x7;
            GPIO = SGPIO.port;
            special_delay();
            
            // apago rgb
            SGPIO.RGB = 0;
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 2: // contador rgb
            if(SGPIO.RGB < 0x7) {
                ++SGPIO.RGB;
            } else {
                SGPIO.RGB = 0x1;
            } 
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 3: // blink luz
            SGPIO.LIGHT = !SGPIO.LIGHT;
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 4: // blink luz + contador rgb
            SGPIO.LIGHT = !SGPIO.LIGHT;
            
            // contador rgb
            if(SGPIO.RGB < 0x7) {
                ++SGPIO.RGB;
            } else {
                SGPIO.RGB = 0x1;
            } 
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 5: // modo flash rgb
            SGPIO.RGB = 0x7;
            GPIO = SGPIO.port;
            __delay_ms(20);
            SGPIO.RGB = 0x0;
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 6: // modo flash luz
            SGPIO.LIGHT = 1;
            GPIO = SGPIO.port;
            __delay_ms(20);
            SGPIO.LIGHT = 0;
            GPIO = SGPIO.port;
            special_delay();
            break;
        case 7: // modo flash luz + rgb
            SGPIO.RGB = 0x7;
            SGPIO.LIGHT = 1;
            GPIO = SGPIO.port;
            __delay_ms(20);
            SGPIO.RGB = 0x0;
            SGPIO.LIGHT = 0;
            GPIO = SGPIO.port;
            special_delay();
            break;
    }
}

void main(void) {
    unsigned char prev_cmd = NEC_NO_COMMAND;
    
    setup();
    
    process_light(); // enciende luz por defecto
    
    while(1) {
        if(nec_cmd != NEC_NO_COMMAND) {
            if(nec_cmd == NEC_MOMENTARY_ON) {
                // repetir prev_cmd
            }
            
            switch(nec_cmd) {
                case 0x72: // red btn
                    process_off();
                    break;
                case 0x71: // green btn
                    process_special();
                    break;
                case 0x63: // yellow btn
                    if(special_state > 0) {
                        if(special_delay_state > 0) --special_delay_state;
                    } else {
                        process_rgb();
                    }
                    break;
                case 0x61: // blue btn
                    if(special_state > 0) {
                        if(special_delay_state < 9) ++special_delay_state;
                    } else {
                        process_light();
                    }
                    break;
            }

            prev_cmd = nec_cmd;
            nec_cmd = NEC_NO_COMMAND;
        }
        
        process_dinamic_modes();
    }
}
