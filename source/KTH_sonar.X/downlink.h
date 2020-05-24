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
#include "dsp.h"
  
void printString( char *string );
void printIntArray(int *arr, int arr_len);
void printLongIntArray(long int *arr, int arr_len);
void printInt16_tArray(int16_t *arr, int arr_len);
void printDoubleArray(const double *arr, int arr_len);
void printInt( int i );
void printInt16c_delim(int16c val);
void printLongInt(long int i);
void printChar(char c);
void printFloat( float f );
void printDouble( double f );
void printEL(void);
void printDelim(void);
void set_comdelay(int s);
void transmit_last_recorded_depth(void);
void transmit_storage_vector(void);
void transmit_correlation_values(void);
void transmit_time_values(void);
void transmit_sonar_settings(void);

int get_comdelay(void);
//void transmitExtractOfAllVariables(char *UARToutbuffer, int comdelay);

// Yaolin's debug, can be deleted
void transmit_storage_vector_N(int N);
void transmit_storage_vector_dc_N(int N);
void transmit_storage_vector_dc(void);
void transmit_storage_vector_test(void);
void transmit_storage_vector_test_N(int N);







#ifdef	__cplusplus
}
#endif

#endif	/* DOWNLINK_H */

