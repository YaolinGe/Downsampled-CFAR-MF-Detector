/* 
 * File:   downlink.h
 * Author: Viktor
 *
 * Created on den 26 juli 2017, 13:50
 */

#ifndef DOWNLINK_H
#define	DOWNLINK_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
void printString( char *string );
void printIntArray(int *arr, int arr_len);
void printInt16_tArray(int16_t *arr, int arr_len);
void printDoubleArray(const double *arr, int arr_len);
void printInt( int i );
void printLongInt(long int i);
void printChar(char c);
void printFloat( float f );
void printDouble( double f );
void printEL(void);
void printDelim(void);
void set_comdelay(int s);
int get_comdelay(void);

#ifdef	__cplusplus
}
#endif

#endif	/* DOWNLINK_H */

