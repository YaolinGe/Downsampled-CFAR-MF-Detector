/* 
 * File:   delay.h
 * Author: Viktor
 *
 * Created on den 24 juli 2017, 15:57
 */

#ifndef DELAY_H
#define	DELAY_H

#ifdef	__cplusplus
extern "C" {
#endif

void delaymicro(int nn);
void delaymillimicron(int nn);
void delaymilli(int nn);
void delay200plus_n_100ns(volatile int n);
#ifdef	__cplusplus
}
#endif

#endif	/* DELAY_H */

