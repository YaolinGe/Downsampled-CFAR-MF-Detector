#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include <stdint.h>
#include "delay.h"
#include "downlink.h"
#include "UART.h"
#include "definitions.h"

#define DEFAULT_BUFFERSIZE 200

// Delay settings
volatile int comdelay = 0;
char downlinkbuffer[DEFAULT_BUFFERSIZE]; //buffer for UART output
/* downlink.c ************************************************
 * This function contains generic downlink functions, that are an
 * interface to io.c, and also downlink functions that send a 
 * package of data (like a package of variables). The generic 
 * downlink functions use specific functions for the chosen 
 * downlink pipe, e.g. UART, and the pipe is chosen by setting 
 * the "downlink_mode_" variables. The generic functions need 
 * if-cases to handle downlinking to different pipes.
 */

/****************************************************************
 ******** Interface between hardware IO communication and   ******
 ******** software IO printing (printing stuff in the code) ******
 ****************************************************************/
void printString(char *string) {
    transmitStringUART(downlinkbuffer, string);
}

void printIntArray(int *arr, int arr_len) {
    int count = 0;
    do {
        transmitIntUART(downlinkbuffer, arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printInt16_tArray(int16_t *arr, int arr_len) {
    int count = 0;
    do {
        transmitIntUART(downlinkbuffer, (int)arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printDoubleArray(const double *arr, int arr_len) {
    int count = 0;
    do {
        transmitDoubleUART(downlinkbuffer, arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printInt(int i) {
    transmitIntUART(downlinkbuffer, i);
}

void printLongInt(long int i) {
    transmitLongIntUART(downlinkbuffer, i);
}

/*void printInt_wo_delim(int i) {
    transmitIntUART_wo_delim(downlinkbuffer, i);
}*/

void printChar(char c) {
    transmitCharUART(downlinkbuffer, c);
}

void printFloat(float f) {
    transmitFloatUART(downlinkbuffer, f);
}

void printDouble(double f) {
    transmitDoubleUART(downlinkbuffer, f);
}

void printEL(void) {
    transmitEndLineUART(comdelay);
}

void printDelim(void){
    writeDelimiterToUART();
}

void set_comdelay(int c) {
    comdelay = c;
}

int get_comdelay(void){
    return comdelay;
}
