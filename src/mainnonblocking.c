/*
 * EJEMPLO EN PIC12F629 DE CAPTURA DE COMANDOS NEC DESDE SENSOR IR EN MODO NO BLOQUEANTE
 * 
 * Se requiere:
 * - Timer0
 * - Activar interrupcion a Pin 3 (input)
 * - Calibrado para usar clock interno de 4MHZ y FOSC/4 y PRESCALER 1:128
 * 
 * Descripcion:
 * En este modo se activa la interrupcion al pin defindo como NEC_IRSENSOR
 * para esperar un flanco de señal (edge up o down segun NEC_EDGE_UP), cuando se
 * dispara la interrupcion se invoca al metodo nec_decode() que intenta 
 * decodificar el comando y guardarlo en la variable nec_cmd.
 * 
 * La variable nec_cmd debe ser monitoreada constantemente en un loop de main() 
 * para atender el comando.
 * 
 * File:   mainnonblocking.c
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

/**
 * Captura de comando NEC por interrupcion del pin IRSENSOR
 */
char nec_cmd;
void __interrupt() ir_intr( void ) {
    if (INTCONbits.GPIF) {
        if(nec_cmd == NEC_NO_COMMAND) { // IMPORTANTE: si esta libre la variable, buscamos otro comando
            nec_cmd = nec_decode();
        }
        INTCONbits.GPIF = 0; // limpia flag para recibir otra interrupcion
    }
}

void setup() {
    TRISIO = 0b00001000; // GP3 input, el resto output
    IOC = 0b00001000; // GP3 atiende interrupciones
    GPIO = 0; // todas las salidas a 0
    
    INTCONbits.GPIE = 1;	// external interrupt on GPIO3 pin(4) is enabled
    INTCONbits.GPIF = 0;	// clear the external interrrupt flag
    INTCONbits.PEIE = 1;    // peripheral intrrupt enable
    INTCONbits.GIE = 1;     // GLOBL interrupt enable
    
    OPTION_REGbits.T0CS = 0;    // usar clock interno (CLKOUT)
    OPTION_REGbits.PSA = 0;     // usar prescaler 
    OPTION_REGbits.PS = NEC_PRESC_TMR0;
}

void main(void) {
    char prev_cmd = NEC_NO_COMMAND;
    
    setup();
    
    while(1) {
        if(nec_cmd != NEC_NO_COMMAND) {
            if(nec_cmd == NEC_MOMENTARY_ON) {
                // repetir prev_cmd
            }
            
            switch(nec_cmd) {
                case 0x72: // red btn
                    GPIObits.GP0 = !GPIObits.GP0;
                    break;
                case 0x71: // green btn
                    GPIObits.GP1 = !GPIObits.GP1;
                    break;
                case 0x63: // yellow btn
                    GPIObits.GP2 = !GPIObits.GP2;
                    break;
                case 0x61: // blue btn
                    GPIObits.GP4 = !GPIObits.GP4;
                    break;
            }

            prev_cmd = nec_cmd;
            nec_cmd = NEC_NO_COMMAND; // IMPORTANTE: resetea la variable para recibir un nuevo comando
        }
    }
}
