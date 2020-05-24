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

#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
#include "dsp.h"
#include <math.h>

// Subtract const from array ------
void array_add_const_16_t(int16_t* arr, int16_t constant, int len);

// ----- Sum -----------------
int32_t sum_16_t(int16_t* arr, int len);
int32_t sum_16c_real(int16c* arr, int len);
int32_t sum_16c_imag(int16c* arr, int len);
    
// ----- Absolute value of complex variable
int16_t abs_16c(int16c val);
int32_t abs_32c(int32c val);

// Signed max/min functions ---------------
int maxind_of_arr_double(double* arr,int len);
int minind_of_arr_double(double* arr,int len);
int maxind_of_arr_int(int* arr,int len);
int minind_of_arr_int(int* arr,int len);

// Unsigned max/min functions -------------
int abs_maxind_of_arr_double(double* arr,int len);
int abs_minind_of_arr_double(double* arr,int len);
int abs_maxind_of_arr_int(int* arr,int len);
int abs_minind_of_arr_int(int* arr,int len);
int16_t abs_maxval_of_arr_int16c(int16c* arr,int len);
int32_t abs_maxval_of_arr_int32c(int32c* arr,int len);

// Range functions -------------------------
void range_double(double* res, int len, double val1, double val2);
void range_int(int* res, int len, int val1, int val2);
void range_longint(long int* res, int len, long int val1, long int val2);

// Vector rotation -------------------------
void arrayrot_int(int* arr, int len, int rot);
void arrayrot_longint(long int* arr, int len, int rot);
void arrayrot_double(double* arr, int len, int rot);

// Complex multiplication, convolution, correlation -----
int32c mult_complex_val_16c(int16c val1, int16c val2);
int32c corr_complex_val_16c(int16c val1, int16c val2);
int32c mult_complex_val_32c(int32c val1, int32c val2);
int32c corr_complex_val_32c(int32c val1, int32c val2);
void mult_complex_arr(int32c* res, int16c* arr1, int16c* arr2, int len);
void corr_complex_arr(int32c* res, int16c* arr1, int16c* arr2, int len);
void rescale_complex2max_int32c(int32c* arr, int len, int scale );
void conv_prepadded_32c(int32c* res, int reslen, int32c* seq, int32c* filt, int Ntaps);

#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
