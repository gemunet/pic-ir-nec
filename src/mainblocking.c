/*
 * EJEMPLO EN PIC12F629 DE CAPTURA DE COMANDOS NEC DESDE SENSOR IR EN MODO BLOQUEANTE
 * 
 * Se requiere:
 * - Timer0
 * - Calibrado para usar clock interno de 4MHZ y FOSC/4 y PRESCALER 1:128
 * 
 * Descripcion:
 * En este modo se llama nec_wait_command() y bloquea el flujo esperando un 
 * cambio en el pin NEC_IRSENSOR que intentara decodificar y retornara con
 * un comando valido o NEC_NO_COMMAND en caso de invalido.
 * 
 * File:   mainblocking.c
 * Author: gemu
 *
 * Created on 30 de junio de 2018, 15:46
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

void setup() {
    TRISIO = 0b00001000; // GP3 input, el resto output
    GPIO = 0; // todas las salidas a 0
    
    OPTION_REGbits.T0CS = 0;    // usar clock interno (CLKOUT)
    OPTION_REGbits.PSA = 0;     // usar prescaler 
    OPTION_REGbits.PS = NEC_PRESC_TMR0;
}

void main(void) {
    char prev_cmd = 0;
    char nec_cmd = 0;
    
    setup();
    
    while(1) {
        // bloquea mientras monitorea actividad en el pin IRSENSOR
        nec_cmd = nec_wait_command();
        
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
        }
    }
}
