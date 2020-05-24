#include "sonar_math.h"
#include "downlink.h"
#include "SONARconfig.h"

// Const operation on array ------
void array_add_const_16_t(int16_t* arr, int16_t constant, int len){
    int i;
   for (i=0;i<len;i++){
       arr[i]=arr[i]+constant;
    }
}

// ----- Sum -----------------
int32_t sum_16_t(int16_t* arr, int len){
    int32_t out=0;
    int i;
    for (i=0;i<len;i++){
        out=out+arr[i];
    }
    return out;
}

int32_t sum_16c_real(int16c* arr, int len){
    int32_t out=0;
    int i;
    for (i=0;i<len;i++){
        out=out+arr[i].re;
    }
    return out;
}

int32_t sum_16c_imag(int16c* arr, int len){
    int32_t out=0;
    int i;
    for (i=0;i<len;i++){
        out=out+arr[i].im;
    }
    return out;
}

// ----- Absolute value of complex variable
int16_t abs_16c(int16c val){
    return (int16_t) sqrt(pow((double) val.re,(double)2)+pow((double)val.im,(double)2));
}
int32_t abs_32c(int32c val){
    return (int32_t) sqrt(pow((double) val.re,(double)2)+pow((double)val.im,(double)2));
}


// ----- Signed max/min functions
// Return maximum of array
int maxind_of_arr_double(double* arr,int len){
    // Note: largest value of a 32bit double here is 2147483500.000
    // Note: if several maxima, returns the first
    double max=(double)-2147483647; // -(2^31-1)
    int imax=0;
    int i;
    for(i=0;i<len;i++){
        if(max<arr[i]){
            max=arr[i];
            imax=i;
        }
    }
    return imax;
}

// Return minimum of array
int minind_of_arr_double(double* arr,int len){
    // Note: largest value of a 32bit double here is 2147483500.000
    // Note: if several minima, returns the first
    double min=(double)2147483647; // 2^31-1
    int imin=0;
    int i;
    for(i=0;i<len;i++){
        if(min>arr[i]){
            min=arr[i];
            imin=i;
        }
    }
    return imin;
}

// Return maximum of array
int maxind_of_arr_int(int* arr,int len){
    // Note: if several maxima, returns the first
    int max=(int)-32767; // -(2^15-1)
    int imax=0;
    int i;
    for(i=0;i<len;i++){
        if(max<arr[i]){
            max=arr[i];
            imax=i;
        }
    }
    return imax;
}

// Return minimum of int array
int minind_of_arr_int(int* arr,int len){
    // Note: if several minima, returns the first
    int min=(int)32767; // 2^15-1
    int imin=0;
    int i;
    for(i=0;i<len;i++){
        if(min>arr[i]){
            min=arr[i];
            imin=i;
        }
    }
    return imin;
}


// ----- Unsigned max/min functions
int abs_maxind_of_arr_double(double* arr,int len){
    // Note: largest value of a 32bit double here is 2147483500.000
    // Note: if several maxima, returns the first
    double max=(double)0; 
    int imax=0;
    double cmp;
    int i;
    for(i=0;i<len;i++){
        
        if(arr[i]<0){
            cmp=-arr[i];
        }
        else{
            cmp=arr[i];
        }
        if(max<cmp){
            max=cmp;
            imax=i;
        }
    }
    return imax;
}

// Return minimum of array
int abs_minind_of_arr_double(double* arr,int len){
    // Note: largest value of a 32bit double here is 2147483500.000
    // Note: if several minima, returns the first
    double min=(double)2147483647; // 2^31-1
    int imin=0;
    double cmp;
    int i;
    for(i=0;i<len;i++){
        
        if(arr[i]<0){
            cmp=-arr[i];
        }
        else{
            cmp=arr[i];
        }
        if(min>cmp){
            min=cmp;
            imin=i;
        }
    }
    return imin;
}

// Return maximum of array
int abs_maxind_of_arr_int(int* arr,int len){
    // Note: if several maxima, returns the first
    int max=(int)0;
    int imax=0;
    int cmp;
    int i;
    for(i=0;i<len;i++){
        
        if(arr[i]<0){
            cmp=-arr[i];
        }
        else{
            cmp=arr[i];
        }
        if(max<cmp){
            max=cmp;
            imax=i;
        }
    }
    return imax;
}

// Return minimum of array
int abs_minind_of_arr_int(int* arr,int len){
    // Note: if several minima, returns the first
    int min=(int)32767; // 2^15-1
    int imin=0;
    int cmp;
    int i;
    for(i=0;i<len;i++){
        
        if(arr[i]<0){
            cmp=-arr[i];
        }
        else{
            cmp=arr[i];
        }
        if(min>cmp){
            min=cmp;
            imin=i;
        }
    }
    return imin;
}


// Generate values in range
void range_double(double* res, int len, double val1, double val2){
    // Note: largest value of a 32bit double here is 2147483500.000
    
    // Check for valid range
    if(val1<-(double)2147483647){ // 2^31-1
        val1=-(double)2147483647;
    } else if(val1>(double)2147483647){
        val1=(double)2147483647;
    }
    if(val2<-(double)2147483647){
        val2=-(double)2147483647;
    } else if(val2>(double)2147483647){
        val2=(double)2147483647;
    }   
    
    double range=val2-val1;
    double du=range/((double)len-1);
    
    int i;
    if(range!=0){
        // Calculate range by increments
        for(i=0;i<len;i++){
            res[i]=val1+i*du;
        }
    }
    else{
        // Zero range; set vector to zeros
        for(i=0;i<len;i++){
            res[i]=0;
        }
    }
    
}

// Generate values in range
void range_int(int* res, int len, int val1, int val2){
   
    // Check for valid range 
    if(val1<-(int)32767){ // 2^15-1
        val1=-(int)32767;
    } else if(val1>(int)32767){
        val1=(int)32767;
    }
    if(val2<-(int)32767){
        val2=-(int)32767;
    } else if(val2>(int)32767){
        val2=(int)32767;
    }   
    
    int range=val2-val1;
    int du=(int)(((double)range)/((double)(len-1)));

    int i;
    if(range!=0){
        // Calculate range by increments
        for(i=0;i<len;i++){
            res[i]=val1+i*du;
        }
    }
    else{
        // Zero range; set vector to zeros
        for(i=0;i<len;i++){
            res[i]=0;
        }
    }
    
}

// Generate values in range
void range_longint(long int* res, int len, long int val1, long int val2){
   
    // Check for valid range 
    if(val1<-(long int)2147483647){ // 2^31-1
        val1=-(long int)2147483647;
    } else if(val1>(long int)2147483647){
        val1=(long int)2147483647;
    }
    if(val2<-(long int)2147483647){
        val2=-(long int)2147483647;
    } else if(val2>(long int)2147483647){
        val2=(long int)2147483647;
    }   
    
    long int range=val2-val1;
    long int du=(long int)(((double)range)/((double)(len-1)));

    int i;
    if(range!=0){
        // Calculate range by increments
        for(i=0;i<len;i++){
            res[i]=val1+i*du;
        }
    }
    else{
        // Zero range; set vector to zeros
        for(i=0;i<len;i++){
            res[i]=0;
        }
    }
    
}

// Rotation of a vector
void arrayrot_int(int* arr, int len, int rot){
    int tmp[len];
    // Check if rotation more than once
    if(rot>=len){
        rot=rot-len; // Periodic with len
    }else if(-rot>=len){
        rot=rot+len;
    }
    int i;
    int irot;
    for(i=0;i<len;i++){
        
        irot=i+rot;
        if(irot>=len){
            // Rotates outside array on right side
            irot=irot-len;
        }
        if(irot<0){
            // Rotates outside array on left side
            irot=len+irot;
        }
        tmp[irot]=arr[i];
    }
    // Overwrite old array
    for(i=0;i<len;i++){
        arr[i]=tmp[i];
    }
    
}

// Rotation of a vector
void arrayrot_longint(long int* arr, int len, int rot){
    long int tmp[len];
    // Check if rotation more than once
    if(rot>=len){
        rot=rot-len; // Periodic with len
    }else if(-rot>=len){
        rot=rot+len;
    }
    int i;
    int irot;
    for(i=0;i<len;i++){
        
        irot=i+rot;
        if(irot>=len){
            // Rotates outside array on right side
            irot=irot-len;
        }
        if(irot<0){
            // Rotates outside array on left side
            irot=len+irot;
        }
        tmp[irot]=arr[i];
    }
    // Overwrite old array
    for(i=0;i<len;i++){
        arr[i]=tmp[i];
    }
    
}

// Rotation of a vector
void arrayrot_double(double* arr, int len, int rot){
    double tmp[len];
    // Check if rotation more than once
    if(rot>=len){
        rot=rot-len; // Periodic with len
    }else if(-rot>=len){
        rot=rot+len;
    }
    int i;
    int irot;
    for(i=0;i<len;i++){
        
        irot=i+rot;
        if(irot>=len){
            // Rotates outside array on right side
            irot=irot-len;
        }
        if(irot<0){
            // Rotates outside array on left side
            irot=len+irot;
        }
        tmp[irot]=arr[i];
    }
    // Overwrite old array
    for(i=0;i<len;i++){
        arr[i]=tmp[i];
    }
    
}

/* FUNCTIONS ABOVE HAVE BEEN TESTED */

// Complex multiplication, convolution, correlation -----

int32c mult_complex_val_16c(int16c val1, int16c val2){
    // Multiply val1 with val2
    int32c tmp; 
    tmp.re= (int32_t) (val1.re*val2.re-val1.im*val2.im);
    tmp.im= (int32_t) (val1.re*val2.im+val2.re*val1.im);
    return tmp;
}

int32c corr_complex_val_16c(int16c val1, int16c val2){
    // Correlate val1 with val2 (val2 is conjugated)
    int32c tmp; 
    tmp.re= (int32_t) (val1.re*val2.re+val1.im*val2.im);
    tmp.im= (int32_t) (val2.re*val1.im-val1.re*val2.im);
    return tmp;
}
int32c mult_complex_val_32c(int32c val1, int32c val2){
    // Multiply val1 with val2
    int32c tmp; 
    tmp.re=val1.re*val2.re-val1.im*val2.im;
    tmp.im=val1.re*val2.im+val2.re*val1.im;
    return tmp;
}
int32c corr_complex_val_32c(int32c val1, int32c val2){
    // Correlate val1 with val2 (val2 is conjugated)
    int32c tmp; 
    tmp.re=val1.re*val2.re+val1.im*val2.im;
    tmp.im=val2.re*val1.im-val1.re*val2.im;
    return tmp;
}

void mult_complex_arr(int32c* res, int16c* arr1, int16c* arr2, int len){
    int i;
    for (i=0;i<len;i++){
        res[i]=mult_complex_val_16c(arr1[i],arr2[i]);
    }
}

void corr_complex_arr(int32c* res, int16c* arr1, int16c* arr2, int len){
    int i;
    for (i=0;i<len;i++){
        res[i]=corr_complex_val_16c(arr1[i],arr2[i]); // arr2 gets conjugated
    }
}

void conv_prepadded_32c(int32c* res, int reslen, int32c* seq, int32c* filt, int Ntaps){
    // This function assumes that seq has been pre-padded with floor(Ntaps/2) 
    // values before, and ceil(Ntaps/2)-1 taps after
    int i; int j; 
    int32c intersum,tmp;
    intersum.re=0; 
    intersum.im=0;
    for (i=0;i<reslen;i++){
        // Convolve seq[i-Ntaps] to seq[i] with filt, and store in res[i]
        for (j=0;j<Ntaps;j++){
            tmp=mult_complex_val_32c(seq[i+j],filt[j]);
            intersum.re=intersum.re+tmp.re;
            intersum.im=intersum.im+tmp.im;
        }
        res[i]=intersum;
        intersum.re=0; 
        intersum.im=0;
    }
}

void rescale_complex2max_int32c(int32c* arr, int len, int32_t scale ){
    int32_t max=abs_maxval_of_arr_int32c(arr, len);
    
    if (max<MAX_SCALE)
      max = MAX_SCALE;

    if( max>scale ){
        // To large for range, lower maximum to scale
        int i;
//        print_array_int32c(arr, "RESCALE_BEFORE_TEST", len);
        for (i=0;i<len;i++)
        {
            arr[i].re=(arr[i].re*scale)/max;
            arr[i].im=(arr[i].im*scale)/max;
        }
//        print_array_int32c(arr, "RESCALE_AFT_TEST", len);
    } else{
        // Scale is ok, do nothing
        return;
    }
    
}

int16_t abs_maxval_of_arr_int16c(int16c* arr,int len){
    // Note: if several maxima, returns the first
    int16_t max=0;
    int16_t cmp;
    int i;
    for(i=0;i<len;i++){
        // Check the real part
        if(arr[i].re<0){
            cmp=-arr[i].re;
        }
        else{
            cmp=arr[i].re;
        }
        if(max<cmp){
            max=cmp;
        }
        // Check the imaginary part
        if(arr[i].im<0){
            cmp=-arr[i].im;
        }
        else{
            cmp=arr[i].im;
        }
        if(max<cmp){
            max=cmp;
        }    
    }
    return max;
}

int32_t abs_maxval_of_arr_int32c(int32c* arr,int len){
    // Note: if several maxima, returns the first
    int32_t max=0;
    int32_t cmp;
    int i;
    for(i=0;i<len;i++){
        // Check the real part
        if(arr[i].re<0){
            cmp=-arr[i].re;
        }
        else{
            cmp=arr[i].re;
        }
        if(max<cmp){
            max=cmp;
        }
        // Check the imaginary part
        if(arr[i].im<0){
            cmp=-arr[i].im;
        }
        else{
            cmp=arr[i].im;
        }
        if(max<cmp){
            max=cmp;
        }    
    }
    return max;
}

