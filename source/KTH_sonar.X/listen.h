/* 
 * File:   listen.h
 * Author: Viktor
 *
 * Created on den 9 augusti 2017, 13:27
 */

#ifndef LISTEN_H
#define	LISTEN_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>     // Necessary for int16_t declaration
void raw_listen( int16_t *raw_ADC, int raw_ADC_len );
void listen_noise();
void listen(int N);
float get_noiserms(void);
void set_mean_noise(int mean);
int get_mean_noise(void);
double get_standard_deviation(void);



// debug functions, -- Yaolin
void listen_full_storage(void);
void listen_N_samples(int N);
void listen_for_downconversion(int Nframes);
void listen_with_fixed_threshold(int Nframes);


#ifdef	__cplusplus
}
#endif

#endif	/* LISTEN_H */

