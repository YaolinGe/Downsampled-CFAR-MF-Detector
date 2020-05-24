/* 
 * File:   memory.h
 * Author: vikto
 *
 * Created on October 4, 2018, 7:44 PM
 */

#ifndef MEMORY_H
#define	MEMORY_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//#include <dsplib_dsp.h>
#include "dsp.h"
#include <stdint.h>
    
// Define memory size constants    
#define CORR_SAVE_LEN 2*NFFT
#define TIME_SAVE_LEN 2*NFFT
    
    
int32_t* get_storage_vector(void);
int get_storage_windex(void);
void set_storage_windex(int ind); // windex is write index
int get_storage_rindex(void); // rindex is read index
void set_storage_rindex(int ind);
int get_storage_vector_len(void);
void set_storage_vector_len(int len);
void clear_storage(void);

int16c* get_scratch16(void);            //holds FFT coefficients (16 bit)
int32c* get_scratch32(void);            //holds FFT coefficients (32 bits)
int32c* get_refsig_time(void);          //holds input template for FFT in Q15 format
int32c* get_refsig_freq(void);     //holds output template from FFT in Q15 format


int32c* get_ctime(void);              //holds MF input (32bit due to large values)
int32c* get_cfreq(void);             //holds MF output (32bit due to large values)
int32c* get_twiddles32(void);

//void insert_into_storage(int32_t val);
int16_t read_from_storage(void);

void allocate_storage_vector(int len);
void deallocate_storage_vector(void);

int32_t* get_corr_save(void);
int32_t* get_time_save(void);
int get_corr_save_len(void);
int get_time_save_len(void);

// Debug ======================== can be deleted ===============================
int get_Nstored(void);
void insert_into_storage(int32_t val);
void insert_into_storage_dc(int32_t val);
void insert_into_storage_test(int32_t val);

int32_t* get_storage_vector_dc(void);
int32_t* get_storage_vector_test(void);


int32_t* get_storage_vector_dc(void);
int get_storage_windex_dc(void);
void set_storage_windex_dc(int ind); // windex is write index
int get_storage_rindex_dc(void); // rindex is read index
void set_storage_rindex_dc(int ind);
int get_storage_vector_dc_len(void);
void set_storage_vector_dc_len(int len);
void clear_storage_dc(void);
void clear_storage_test(void);


int get_storage_windex_test(void);
void set_storage_windex_test(int ind); // windex is write index
int get_storage_rindex_test(void); // rindex is read index
void set_storage_rindex_test(int ind);
int get_storage_vector_test_len(void);
void set_storage_vector_test_len(int len);

void print_refsig(void);

int32c* get_scratch32_dc(void);
int32c* get_twiddles32_dc(void);





#ifdef	__cplusplus
}
#endif

#endif	/* MEMORY_H */

