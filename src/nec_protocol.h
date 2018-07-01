/* 
 * CAPTURA DE COMANDOS NEC DESDE SENSOR IR PARA PIC
 * 
 * Esta libreria permite capturar comandos NEC emitidos desde un control
 * remoto infrarojo.
 * 
 * Posee dos modos de captura, bloqueante y no bloqueante:
 * 
 * - Bloqueante: se invoca a nec_wait_command() y este detiene el flujo del
 *   programa por tiempo indefinido hasta recibir un comando valido o invalido
 *   desde el pin IR.
 * 
 * - No bloqueante: Si detectamos actividad en el pin IR, ya sea por una interrupcion
 *   o algun otro metodo, invocamos inmediatamente a nec_decode() quien intentara
 *   decodificar el comando y retornara el resultado.
 * 
 * Este archivo esta configurado para funcionar correctamente con PIC12F629, pero
 * se puede adaptar a cualquier microchip modificando la calibracion del prescaler
 * y el timer en la seccion inferior.
 * Como el tiempo de la señal a detectar puede NO SER EXACTA se debe definir un
 * margen de tolerancia inferior (ej. NEC_9MS_TOLERANCE) y superior (ej. NEC_9MS_TMR0)
 * que puede variar segun la velocidad de procesamiento del microcontrolador.
 * 
 * File:   nec_protocol.h
 * Author: gemu
 */

#ifndef XC_NEC_PROTOCOL_H
#define	XC_NEC_PROTOCOL_H

#include <xc.h> // include processor files - each processor file is guarded.  


// SECCION CALIBRACION
#define NEC_PRESC_TMR0          0b110 // 1/128
#define NEC_9MS_TMR0            170 // 9ms
#define NEC_9MS_TOLERANCE       235 // margen de tolerancia antes de overflow 
#define NEC_4_5MS_TMR0          212 // 4.5ms
#define NEC_4_5MS_TOLERANCE     240 // margen de tolerancia antes de overflow 
#define NEC_1_69MS_TMR0         236 // 1.69ms
#define NEC_1_69MS_TOLERANCE    245 // margen de tolerancia antes de overflow
#define NEC_560US_TMR0          250 // 560us
// FIN SECCION DE CALIBRACION


#define NEC_NO_COMMAND          0 // no hay comando o comando invalido
#define NEC_MOMENTARY_ON        255 // boton se mantiene presionado
#define NEC_EDGE_UP             0 // 0 = iniciar el detectar flanco de bajada, 1 = flanco de subida
#define NEC_IRSENSOR            GPIObits.GP3 // pin de entrada del sensor

/**
 * Variable global para recibir y procesar comandos NEC
 */
struct {
    unsigned char address; // direccion
    unsigned char address_inv; // direccion invertida
    unsigned char command; // comando
    unsigned char command_vrf; // comando para verificar (se guarda no invertido)
} nec_packet;

/**
 * Decodifica un comando ya detectado
 * @return  NEC_NO_COMMAND si no hay comando, 
 *          NEC_MOMENTARY_ON si es repeticion, 
 *          > 0 codigo del comando.
 */
unsigned char nec_decode();

/**
 * Espera por un comando infinitamente
 * @return  NEC_NO_COMMAND si no hay comando,
 *          NEC_MOMENTARY_ON si es repeticion, 
 *          > 0 codigo del comando.
 */
unsigned char nec_wait_command();

#endif

