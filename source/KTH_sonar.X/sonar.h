/* 
 * File:   sonar.h
 * Author: Viktor
 *
 * Created on den 26 juli 2017, 16:24
 */

#ifndef SONAR_H
#define	SONAR_H

#ifdef	__cplusplus
extern "C" {
#endif
   
//#include <stdint.h>     // Necessary for int16_t declaration
    
void operateSonar(void);
void operateVoltageTracking(void);
void set_np(int p);
void set_voltagetolerance(int v);
void set_voltage(int v);
void set_frequency(float frequency);
void set_soundspeed(int soundspeed);
void set_BW(int b);
int get_np(void);
int get_voltage(void);
float get_frequency(void);
double get_last_recorded_depth(void);
void set_last_recorded_depth(double d);
long int get_last_recorded_depth_correlation(void);
void set_last_recorded_depth_correlation(long int d);
double get_depth_min(void);
double get_depth_max(void);
void set_depth_min(double dmin);
void set_depth_max(double dmax);
void set_last_sound_pressure(double sp);
double get_last_sound_pressure(void);
double get_max_range(void);
int get_bandwidth(void);
void set_bandwidth(int bw);


// yaolin debug, can be removed =====
int get_center_freq(void);
void set_fixed_threshold(int32_t threshold);
int get_fixed_threshold(void);
void set_scale_factor(int scale);



#ifdef	__cplusplus
}
#endif

#endif	/* SONAR_H */

