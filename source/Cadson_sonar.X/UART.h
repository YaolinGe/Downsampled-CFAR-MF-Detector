/* 
 * File:   UART.h
 * Author: vikto
 *
 * Created on December 17, 2017, 7:17 PM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

void writeBufferToUART(char *buffer);
void writeDelimiterToUART(void);
void writeEndLineToUART(void);
void transmitFloatUART(char *buffer, float param);
void transmitDoubleUART(char *buffer, double param);
void transmitIntUART(char *buffer, int param);
void transmitLongIntUART(char *buffer, long int param);
//void transmitIntUART_wo_delim(char *buffer, int param);
void transmitCharUART(char *buffer, char param);
void transmitStringUART(char *buffer, char *param);
void transmitEndLineUART(int comdelay);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

