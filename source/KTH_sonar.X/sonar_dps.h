/* 
 * File:   sonar_dps.h
 * Author: vikto
 *
 * Created on October 5, 2018, 12:13 PM
 */

#ifndef SONAR_DPS_H
#define	SONAR_DPS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//#include <dsplib_dsp.h>
#include "dsp.h"

// Macros for setting window function in windowing() function
#define WINDOW_NO_WINDOW 0 
#define WINDOW_PARZEN 1 
#define WINDOW_WELCH 2
#define WINDOW_HANNING 3 
#define WINDOW_HAMMING 4
#define WINDOW_BLACKMAN 5
#define WINDOW_STEEPER 6
    
int32c correlate16(int16c s1,int16c s2);
double parzen(int i, int nn); 
double welch(int i, int nn);
double hanning(int i, int nn);
double hamming(int i, int nn);  
double blackman(int i, int nn);
double steeper(int i, int nn);
void windowing(int n, const double *data, int flag_window, 
        double scale,double *out);

#ifdef	__cplusplus
}
#endif

#endif	/* SONAR_DPS_H */

