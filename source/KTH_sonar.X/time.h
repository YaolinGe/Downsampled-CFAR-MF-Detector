/* 
 * File:   time.h
 * Author: vikto
 *
 * Created on January 17, 2020, 12:58 PM
 */

#ifndef TIME_H
#define	TIME_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h>
    
typedef struct Time{
    int32_t clk;
//    double secs;
} Time;

double time2sec(Time t);
double clk2sec(int32_t t);
int32_t sec2clk(double t);
int32_t sec2sampl(double tmilli, int32_t rate);
int32_t sampl2clk(int Nsampl, int Fs);
double sampl2sec(int Nsample, int Fs);
int32_t get_time_clk(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIME_H */

