/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef SAMPLING_H    /* Guard against multiple inclusion */
#define SAMPLING_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */



/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
#include "dsp.h"
#include "time.h"
#include <stdint.h>
int scale_factor = 35;

// Init functions 
void init_sampling_lp_filters(int FILTVAL);
void init_buffers(void);

// ADC downconversion functions ----------------
void setup_downconversion(long int f0, long int B);
void setup_downconversion_exponential(long int f);
void downconvert_adc_buffer(int Nvals);
//void downconvert_ref_signal(int32c* ref_signal_dc, int16c *cpy, int32c* refsig_time, long int f0, long int B, int Nvals);
//int32c* ref_signal_dc = downconvert_ref_signal(int16c* cpy, int32c* refsig_time,long int fc, long int B, int Nvals);

// Sampling functions
void start_sampling(long int Fs, double timeout);
void stop_sampling(void);
void downsample_overwrite_int32c(int32c* seq, int r1, int r2, int Nr1);
void init_adc_buffer(void);
void setTransducerToSensing(void);

// ADC buffer functions ------------------------
void insert_val_into_adcbuffer(int16_t val);
int16_t read_val_from_adcbuffer(void);
int16_t glean_val_from_adcbuffer(int delay);
int16_t* unload_adc_buffer(int Nvals, int Nbpad, int Nfwpad);
int16_t* unload_adc_buffer_forget(int Nvals, int Nbpad, int Nfwpad);
int16c* unload_adc_buffer_complex(int Nvals, int Nbpad, int Nfwpad);
void clear_buffer(void);
void clear_buffer_dc(void);
void clear_raw_data_buffer(void);


void transfer_data_from_storage_to_adc_buffer(int Nframe);
int32_t calculate_noise_level(int16_t *p, int Nframe);

// Noise variance estimation ------------------
void insert_threshold_into_buffer(int32_t val) ;
//int32_t calculate_noisevar(int16_t *p, int Nframe);
int N_noiseavg_over_time(double time, int Nframe, long int Fs);
void fill_noisvar_buffer(double time, int Nframe, long int Fs);

// Temporary functiosn - will be removed ------------
void buffer_rotate_backward(int N);
int16_t* fwunfold_buff(int Nframe) ;
int32c* fwunfold_bufftime_FFT(int Nframe);
int16_t* fwunfold_buff_dc(int Nframe); 
int get_index_adcbuffer_nextsmpl(void);
int get_index_adcbuffer_oldest(void);
// -----

// Algorithm timeout functions
void set_algorithm_timeout(double timeout);
void reset_algorithm_timeout(void);
char check_algorithm_timeout(void);

// Getters and setters --------------------------

// Downconversion & lp filter ---------------
int get_Nbpad_dcfilt(void);
int get_Nfwpad_dcfilt(void);
int get_Ncpy(void);
int get_Ndc_baseband(void);
int get_Ndc_passband(void);
int get_dc_factor(void);
int get_exceedance(void);
void set_exceedance(int val);

void set_Ndc_baseband(int Nvals);
void set_Ndc_passband(int Nvals);

// Buffer
int get_buffer_clip_flag(void);
int16_t* get_adc_buffer(void);
int get_adc_buffer_len(void);
int16c* get_adc_buffer_dc(void);
int32_t * get_noisevar_buffer(void);
int32c* get_dc_vals_lpfilt(void);

// Noise estimation -------------
int32_t* get_noisevar_buffer(void);
int get_threshold_buffer_valcnt(void);
int16_t * get_raw_data_buffer(void);
int get_raw_data_buffer_len(void);
void print_raw_data_buffer(void);

// Temporary
//int32c* get_buff_unf(void);
//int32c* get_buff_unf_freq(void);
// ----- 

int get_adc_buffer_valcnt(void);
void set_adc_buffer_valcnt(int cnt);

int get_Nframe(void);

// Sampling
void set_sampling_rate(long int Fs);
Time get_t_buffer(void);
char get_new_sampl(void);
void acquire_samples(int N);
void acquire_samples_dcpadded(int N);
void acquire_samples_forget(int N);

// Debug - Yaolin
int32c * get_lpfilt(void);
int get_Ntaps(void);
void print_lpfilt(void); // can be deleted
void print_dc_component(int Ncpy);
void print_dc_vals(int Ncpy);
void print_dc_vals_lpfilt(void);
void set_index_adcbuffer_oldest(int N);
void set_index_adcbuffer_nextsmpl(int N);

int get_index_noise_level_buffer(void);
void set_index_threshold_buffer(int index);

void pre_filter_fixed_threshold_detector(int Nframes);
void pre_filter_adapted_threshold_detector(int Nframes);
//int32_t * Threshold_CV_CFAR(int16_t* p, int T, int G, int offset);
//int32_t Threshold_CA_CFAR(int16_t* p, int T, int G, int offset);
//int32_t * Thresholds_CA_CFAR(int Nframes, int T, int G, int offset);
int32_t Threshold_CA_CFAR(int T, int G, int16_t offset);

int32_t * DC_Thresholds_CA_CFAR(int Nframes, int T, int G, int offset);
int32_t * padded_adc_buffer(int32_t * p, int T, int G, int N, int numOfNframes, int32_t *overlapsaved_values);

void load_file_downconvert_adc_buffer(void);
void test_pulse_listen_downconvert_adc_buffer(void);
void transfer_data_from_storage_to_adc_buffer(int Nframe);
void transfer_data_from_adc_buffer_to_storage(int Nframe);
void save_adc_buffer_dc_to_storage(void);
void print_adc_buffer(void);
void print_adc_buffer_dc(void);
void print_val(char *p, int val);
void print_adc_buffer_valcnt(void);
void print_adc_buffer_Nframe(int Nframe);
void print_val_int(int val);
void print_val_double(double val);

void ini_everything(void);
void ini_ind_adc_buffer(void);
void setup_dc(void);

void update_threshold_buffer(int16_t *p, int Nframe);
int32_t calculate_noise_level(int16_t *p, int Nframe);
void ini_threshold_buffer(void);
int32_t calculate_threshold_avg(void);
void test_noise(void);
int32_t calculate_noise_base_level(int16_t *p, int Nframe);
int32_t calculate_noise_dc_level_dc(int32_t *p, int Nframe);

int32_t get_threshold_avg(int16_t *p, int Nframe);
int adapted_thresholding(int Nframe);

void print_threshold_buffer(void);


void print_start_mark(char *p);
void print_end_mark(char *p);
void test_thresholding(void);
void test_pre_filtering_with_fixed_threshold(int Nframes);
void test_pre_filtering_with_adapted_threshold(int Nframes);
void load_file_matched_filter_thresholding(void);
int fixed_thresholding_dc(int32c * p, int Nframe);
void downconvert_corr_fixed_thresholding(int Nvals);

void test_delay(void);
void print_ind_o(void);
void test_debug_r(void);
int get_adc_buffer_dc_len(void);
void fake_unload_adc_buffer(int Nframe);


// THRESHOLDING FUNCTIONS
int32_t get_fixed_threshold(void);
int fixed_thresholding(int Nframe);





int32c * get_dc_vals(void);
int16c* get_dc_exponential(void);
int get_scale_factor(void);
int get_scale_factor_transmitter(void);
int32_t * get_threshold_buffer(void);
int get_threshold_buffer_len(void);
double get_time_diff(Time t1, Time t2);

void test_unload_buffer_forget(void);

void downconvert_corr_fixed_thresholding(int Nvals);
int downconvert_corr_adapted_thresholding(int Nvals);
void ini_threshold_buffer_dc_corr(int Nframes);
void ini_threshold_buffer_dc_corr_transmitter_side(int Nframes);
int32_t calculate_noise_level_dc(int32_t *p, int Nframe);
void calculate_abs_corr_time(int32_t *abs_corr_time, int32c * ctime, int Nframe);


void update_threshold_buffer_dc(int32_t *p, int Nframe);
int32_t calculate_threshold_dc_avg(void);
int32_t get_threshold_dc_avg(int32_t *p, int Nframe);
int32_t get_fixed_threshold_dc(void);

void send_pulse(long int fc, long int bandwidth, int waveselect);
void test_time_calculation(void);

void beacon(void);
void beacon_2nd(void);
void transmitter(void);
void transmitter_2nd(void);

void test_ringing_effect_bleeding(void);
int max_index_array_int32_t(int32_t *p, int N);
int max_index_array_int16_t(int16_t *p, int N);
int first_exceedance_index_array_int32_t(int32_t *p, int N, int32_t threshold);
void test_interaction(void);
void test_env_sampling(void);
void test_valcnt_calibration(void);
void debug_clk_speed_tracking(void);
void debug_overflow(void);





// =+++++++++++++++++++++++ DEBUG FUNCTION ONLY ++++++++++++++++++++++++++++++++
void debug_centre_frequency_recalibration(void);
void debug_start_delay_recalibration(void);
void debug_ramping_up_delay_calibration(void);
void debug_beacon(void);
void debug_transmitter(void);
void delay_startdelay(void);




// ++++++++++++++++++++++++++ END OF DEBUG FUNCTION ++++++++++++++++++++++++++++

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif 

/* *****************************************************************************
 End of File
 */
