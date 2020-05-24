#ifdef __XC32
#include <xc.h> //Defines special funciton registers, CP0 regs
#endif
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "SONARconfig.h"
char downlink_mode_UART = 1;

#define TRANSMIT_DELAY 20 // My laptop UART gets choked up for 1 micron delay

/****************************************************************
 * Below are functions for downlinking different data 
 * types via UART.
 */
void writeBufferToUART(char *buffer) {
    unsigned int iyu = 0;
    int ugu = 0;
    char bulle = 0;
    iyu = 0;
    do {
        ugu = buffer[iyu];
        bulle = ugu;
        while (U1STAbits.UTXBF);
        U1TXREG = bulle;
        delaymicro(TRANSMIT_DELAY);
        iyu++;
    } while (buffer[iyu] != 0x00);
}

void writeDelimiterToUART(void) {
    while (U1STAbits.UTXBF);
    U1TXREG = DELIM;
}

void writeEndLineToUART(void) {
    // Send CR then LF
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0D;
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0A;
}
/*******************************/

/****************************************************************
 * Below are generic functions for downlinking different data 
 * types via a chosen datalink (UART, WiFi etc.). This is the 
 * interface to io.c (software development print() functions). 
 */
void transmitFloatUART(char *buffer, float param) {

    memset(buffer, 0, strlen(buffer));
    snprintf(buffer, 21, "%0.4f", (double) param);

    writeBufferToUART(buffer);
    //writeDelimiterToUART();
    delaymicro(TRANSMIT_DELAY);
}

void transmitDoubleUART(char *buffer, double param) {

    memset(buffer, 0, strlen(buffer));
//    snprintf(buffer, 21, "%0.4f", param); // "%0.3f" sets number of decimals printed
    snprintf(buffer, 21, "%0.9f", param);

    writeBufferToUART(buffer);
    //writeDelimiterToUART();
    delaymicro(TRANSMIT_DELAY);
}

void transmitIntUART(char *buffer, int param) {
    memset(buffer, 0, strlen(buffer));
    itoa(buffer, param, 10);

    writeBufferToUART(buffer);
    //writeDelimiterToUART();
    delaymicro(TRANSMIT_DELAY);
}

void transmitLongIntUART(char *buffer, long int param) {
    memset(buffer, 0, strlen(buffer));
    itoa(buffer, param, 10);

    writeBufferToUART(buffer);
    //writeDelimiterToUART();
    delaymicro(TRANSMIT_DELAY);
}

/*void transmitIntUART_wo_delim(char *buffer, int param) {
    memset(buffer, 0, strlen(buffer));
    itoa(buffer, param, 10);

    writeBufferToUART(buffer);
    delaymicro(TRANSMIT_DELAY);
}*/

void transmitCharUART(char *buffer, char param) {
    memset(buffer, 0, strlen(buffer));
    itoa(buffer, param, 10);

    writeBufferToUART(buffer);
    delaymicro(TRANSMIT_DELAY);
}

void transmitStringUART(char *buffer, char *param) {
    memset(buffer, 0, strlen(buffer));
    strcat(buffer, param);

    writeBufferToUART(buffer);
    delaymicro(TRANSMIT_DELAY);
}

void transmitEndLineUART(int comdelay) {

    // Send CR then LF
    writeEndLineToUART();
    delaymicro(comdelay); //delay set by command
}