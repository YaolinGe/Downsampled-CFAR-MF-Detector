/* 
 * File:   detection.h
 * Author: Viktor
 *
 * Created on den 15 augusti 2017, 14:09
 */

#ifndef DETECTION_H
#define	DETECTION_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "dsp.h"
#include <stdint.h>
#include "time.h"
    
#ifdef __XC32
#include <xc.h>   
#endif
    
// Some definitions
#define SINE 1
#define CHIRP 2
#define CHIRP_H 3
#define M_SEQ 4
   
void setup_MatchedFilter(double newtimeout,int WAVEFORM);
void detectEchoMatchedFilter(void);
void detectEchoPowerFV(double thr_decimal);
void test_signal_generation(void);

void generate_sine(double* sine, int frequency, int Fs, double T, int Nlen);
void generate_chirp(double* chirp, int f1, int f2, int Fs, double T, int Nlen);
void generate_chirp_H(double* chirp, int f1, int f2, int Fs, double T, int Nlen);
void generate_MSEQ();

//void listen(int Nlisten, int STORE);
void listenc(int Nlisten);

void correlate(int32c* result, int32c* vref, int32c* vcomp, int cstart, int cstop);
void SNRatfreq(int freq);
void adjust_numberOfPulses(void);
float get_soundspeed(void);
//float get_samplingrate(void);
void set_consecutive_hits(int h);
int get_consecutive_hits(void);
void set_consecutive_misses(int m);
int get_consecutive_misses(void);
int get_trig_num(void);
void set_trig_num( int tr );
//int get_thresh_weight(void);
//void set_thresh_weight( int tr );
double get_deadtime(void);
void set_deadtime(double d);
void set_t_samplstart(int tspl);
void set_timeout(double t);
double get_timeout(void);
int get_t_samplstart(void);
void set_Fs(int f);
int get_Fs(void);
int32_t get_mf_detector_thr(void);
void set_amp_detector_thr(int16_t thr);
void set_mf_detector_thr(int32_t thr);
int get_trig_index(void);
int get_trig_index_min(void);
int get_trig_index_max(void);
double* get_waveform(void);
double get_t_frame(void);
double get_detector_extime(void);
int32_t get_minthresh(void);
void set_minthresh(int32_t minthr);
double get_deadtime(void);
//void set_thrscale_nom(int th);
//void set_thrscale_reverb(int th);
//int get_thrscale_nom(void);
//int get_thrscale_reverb(void);
int get_wave_select(void);
void set_wave_select(int ws);

void detectWaveform(void);

void set_t_frame(double tf);
void set_detector_extime(double detext);
int32_t variance_subvector_c(int32c* vector, int istart, int istop, int vectorlen, int scale);
int index_locmin_subvector_c(int32c* vector, int istart, int istop, int vectorlen);
int index_locmax_subvector16(int16_t* vector, int istart, int istop, int vectorlen);
int index_locmax_subvector32(int32_t* vector, int istart, int istop, int vectorlen);
int index_locmax_subvector_c(int32c* vector, int istart, int istop, int vectorlen);
int index_first_exceedence_subvector(int32_t* vector, int32_t threshold, int istart, int istop, int vectorlen);
int index_last_exceedence_subvector16(int16_t* vector, int16_t threshold, int istart, int istop, int vectorlen);
int index_last_exceedence_subvector32(int32_t* vector, int32_t threshold, int istart, int istop, int vectorlen);
void calculate_thresholds16(int16_t* vector, int16_t* thresholds, int Nthresholds, int framestart, int frameend, int vector_len );
void calculate_thresholds32c(int32c* vector, int32_t* thresholds, int Nthresholds, int framestart, int frameend, int vector_len );

void flip_array_int32c(int32c *p, int Nbuffer);
void load_file_matched_filter_corr(void);
void fft32(int32c * signal_freq, int32c * signal_time);
int get_Npulse(void);
void test_load_file_matched_filtering(void);
void matched_filter_dc_setup(void);
void set_Npulse(double nPulse);
void convert16c_to_32c(int32c *out, int16c *in, int Nlen);
void convert32c_to_16c(int16c *out, int32c *in, int Nlen);
void pad_refsig_time(int16c *refsig_padded, int16c *refsig_time, int Nref_passband, int Nbpad, int Nfwpad);
void print_refsig_time(void);
void print_array_int32c(int32c *p, char *s, int N);
void print_array_int16c(int16c *p, char *s, int N);
void print_array_int32_t(int32_t *p, char *s, int N);


void overwrite_array_int32c(int32c *arr1, int32c *arr2, int Nlen);

void fft32_dc(int32c * signal_freq, int32c * signal_time);
void correlate_dc(int32c* result, int32c* vref, int32c* vcomp, int cstart, int cstop);

void matched_filter_fixed_threshold_dc_detector(int Nframes);
void matched_filter_correlation(void);

void ini_array_int32c(int32c *arr, int Nlen);
void test_matched_filtering_with_fixed_threshold_dc(int Nframes);
void downconvert_refsig_time(int32c *refsig_dc_time, int Nref_passband);

void matched_filter_with_adapted_threshold_dc_detector(int Nframes);
void test_matched_filtering_with_adapted_threshold_dc(int Nframes);


int get_Nref_baseband();
void set_Nref_passband(int Nvals);
void set_Nref_baseband(int Nvals);
int get_Nref_passband();

void test_mf_detector(int transducer);
void mf_detector_setup(long int fc);

void set_index_dc_exceedance(int val);
int get_index_dc_exceedance(void);
Time get_time_arrival(void);
void set_time_arrival(Time time);
Time get_time_current(void);
void set_time_current(Time time);
Time get_fixed_time_delay(void);
void set_fixed_time_delay(Time time);



#ifdef	__cplusplus
}
#endif

#endif	/* DETECTION_H */

