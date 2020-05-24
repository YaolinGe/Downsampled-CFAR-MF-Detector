/* 
 * File:   pulses.h
 * Author: Viktor
 *
 * Created on den 24 juli 2017, 11:27
 */

#ifndef PULSES_H
#define	PULSES_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
#define VPLUS 3072      
#define VMINUS 513
#define MIDPLANE_PLUS 512
#define MIDPLANE_MINUS 2048 ///////???? why this value, middle?
    
void send_combined_sequence(int pulses, int voltage, int frequency, int bandwidth);    
void send_chirp_pulse(int pulses, int voltage, int frequency, int bandwidth);
void send_square_pulse( int pulses, int frequency,int voltage );
void send_sinetrain(int pulses, int frequency,int voltage);
void setTransducerToPulsing(void);
void switchPLUS_HIGH(void);
void switchPLUS_LOW(void);
void switchMINUS_HIGH(void);
void switchMINUS_LOW(void);
void set_t_pulsestart(int32_t tps);
int32_t get_t_pulsestart(void);
void set_startdelay(int s);
int get_startdelay(void);
void set_HVsupply(int volt);
void pulse_pause(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PULSES_H */

