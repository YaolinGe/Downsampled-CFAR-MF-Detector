#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include "time.h"
#include "SONARconfig.h"
#include <stdint.h>

double time2sec(Time t){
    return ((double)t.clk)/FCY;
}

double clk2sec(int32_t t){
    return ((double)t)/FCY;
}

int32_t sec2clk(double t){
    return (int32_t)(t*FCY);
}

int32_t sec2sampl(double t, int32_t rate){
    return (int32_t) (t*rate);
}

double sampl2sec(int Nsample, int Fs)
{
    return clk2sec(sampl2clk(Nsample, Fs));
}

int32_t sampl2clk(int Nsampl, int Fs)
{
    // Exact integer division
    int32_t div=(int32_t)Nsampl*(FCY/Fs);
    int32_t quotient=((int32_t)Nsampl*(FCY%Fs))/Fs;
    return div+quotient; 
}

int32_t get_time_clk(void){
    // If changing the timer used for time keeping, only change
    // here 
    return TMR4; 
}