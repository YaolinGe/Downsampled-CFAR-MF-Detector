/* 
 * File:   setup.h
 * Author: Viktor
 *
 * Created on den 24 juli 2017, 14:54
 */

#ifndef SETUP_H
#define	SETUP_H

#ifdef	__cplusplus
extern "C" {
#endif

void setup(void);
void init_modes(void);
void init_uarts(long int baudrate, long int fpb);
void init_interrupts(void);
void init_ports(void);
void init_pps(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SETUP_H */

