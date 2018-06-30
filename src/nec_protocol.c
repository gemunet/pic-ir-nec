/*
 * File:   nec_protocol.c
 * Author: gemu
 *
 * Created on 30 de junio de 2018, 4:43
 */


#include "nec_protocol.h"

char nec_decode() {
    nec_packet.address = 0;
    nec_packet.address_inv = 0;
    nec_packet.command = 0;
    nec_packet.command_vrf = 0;
    
    // start
    TMR0 = NEC_9MS_TMR0;
    INTCONbits.T0IF = 0;
    while(NEC_IRSENSOR == NEC_EDGE_UP && !INTCONbits.T0IF); // edge down
    if(INTCONbits.T0IF) { return NEC_NO_COMMAND; } // error, overflow
    if(TMR0 < NEC_9MS_TOLERANCE) { // error, ha saltado fuera de margen de tiempo
        return NEC_NO_COMMAND;
    }

    TMR0 = NEC_4_5MS_TMR0;
    INTCONbits.T0IF = 0;
    while(NEC_IRSENSOR != NEC_EDGE_UP && !INTCONbits.T0IF); // edge up
    if(INTCONbits.T0IF) { return NEC_NO_COMMAND; } // error, overflow
    if(TMR0 < NEC_4_5MS_TOLERANCE) { // momentary 
        return NEC_MOMENTARY_ON;
    }

    // data
    for(char i=0; i<32; i++) {
        
        TMR0 = NEC_560US_TMR0;
        INTCONbits.T0IF = 0;
        while(NEC_IRSENSOR == NEC_EDGE_UP && !INTCONbits.T0IF); // edge down
        if(INTCONbits.T0IF) { return NEC_NO_COMMAND; } // error, overflow

        TMR0 = NEC_1_69MS_TMR0;
        INTCONbits.T0IF = 0;
        while(NEC_IRSENSOR != NEC_EDGE_UP && !INTCONbits.T0IF); // edge up
        if(INTCONbits.T0IF) { return NEC_NO_COMMAND; } // error, overflow
        
        if(TMR0 < NEC_1_69MS_TOLERANCE) {
            // bit 0
            if(i < 8) {
                nec_packet.address = (nec_packet.address >> 1);
            } 
            else if(i < 16) {
                nec_packet.address_inv = (nec_packet.address_inv >> 1);
            }
            else if(i < 24) {
                nec_packet.command = (nec_packet.command >> 1);
            }
            else {
                nec_packet.command_vrf = (nec_packet.command_vrf >> 1) | 0x80;
            }
        } else {
            // bit 1
            if(i < 8) {
                nec_packet.address = (nec_packet.address >> 1) | 0x80;
            } 
            else if(i < 16) {
                nec_packet.address_inv = (nec_packet.address_inv >> 1) | 0x80;
            }
            else if(i < 24) {
                nec_packet.command = (nec_packet.command >> 1) | 0x80;
            }
            else {
                nec_packet.command_vrf = (nec_packet.command_vrf >> 1);
            }
        }
        
    }
    
    
    // verficacion de error
    if(nec_packet.command != nec_packet.command_vrf) {
        return NEC_NO_COMMAND;
    }
    
    return nec_packet.command;
}

char nec_wait_command() {
    while(NEC_IRSENSOR != NEC_EDGE_UP); // edge up
    return nec_decode();
}
