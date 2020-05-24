#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
//#include <dsplib_dsp.h>
#include <math.h>
#include <string.h>
#include "SONARconfig.h"
#include "sonar.h"
#include "delay.h"
#include "pulse.h"
#include "flags.h"
#include "downlink.h"
#include "detection.h"
#include "memory.h"
#include "sonar_dps.h"
#include "modes.h"
#include "messages.h"
#include "dsp.h"
#include "sampling.h"

///////// DEBUG CAN BE DELETED
#include "commands.h"
#include "sonar_math.h"

#define RESCALE_FXP 1023

//int wave_select=CHIRP; // Keep track of what waveform to use
int wave_select=SINE; // Keep track of what waveform to use
int t_samplstart = 0; // Time point when samling was started

// Move these to detection later
int trig_num = 10;
//int thresh_weight = DEFAULT_DETECTOR_THRESHOLD_WEIGHT;
//volatile int16_t *adc_buffer;
float soundspeed = DEFAULT_SOUNDSPEED;
//float samplingrate = 0;
int consecutive_hits = 0; // Number of consecutive times triggered on echo
int consecutive_misses = 0; // Number of consecutive times without echo

int index_dc_exceedance = 0;
Time time_arrival;
Time time_current;
Time time_wait;
Time fixed_time_delay;
Time t_ref;

// Define timeout from max range
long int timeout = (long int) DEFAULT_SAMPLING_FREQUENCY*(2*((double)DEFAULT_MAX_RANGE)/DEFAULT_SOUNDSPEED);  // Default timeout in clock cycle
// Sampling timeout
long int timeout_sampl=30000;
//int max_range = DEFAULT_MAX_RANGE; 
double deadtime = 0;
//int Fs = DEFAULT_SAMPLING_FREQUENCY;
int Fs = 100000;


int16_t* adc_buffer;
// Make some information on the last pulse available
double Tpulse=0;
int Npulse=0;
int Npulse_dc = 0;   // downconverted number of samples
int Nref_passband = 0;
int Nref_baseband = 0;

#define DEFAULT_NTHRESHOLDS 4 // Recalculate threshold after each NFFT/NTHRESHOLDS

double waveform[NFFT];
double waveform_w[NFFT];    

void setup_MatchedFilter(double timeout,int WAVEFORM)
{
    int i;
    int np = get_np(); // Number of pulses
    int Fs = get_Fs();
    double dt = 1 / (double) Fs;
    int fc = get_frequency(); 
    int B = get_bandwidth();

    double Tp = 1 / (double)fc;
    Tpulse = Tp * (double)np; // One wave period
    Npulse = (int)(Tpulse * (double)Fs); // Total number of points in the pulse

    printString("Tp is "); printDouble(Tp); printDelim(); printEL();
    printString("1/fc is "); printDouble(1/fc); printDelim(); printEL();
    printString("Tpulse is "); printDouble(Tpulse); printDelim(); printEL();
    printString("Npulse is "); printDouble(Npulse); printDelim(); printEL();

    printString("Npulse is "); printInt(Npulse); printDelim(); printEL();
    print_val("np is ", np);
    print_val("Fs is ", Fs);
    print_val("fc is ", fc);
    print_val("B is ", B);
    printString("Tpulse*1000 is"); print_val_double(Tpulse*1000); printEL();

// ======================= TRUNCATOIN ERROR ====================================
//    printString("TEST VALUE IS ");
//    printInt(160000*0.00025); printDelim(); printEL();
//    printDouble(Npulse); printDelim(); printEL();
//    printInt(Npulse); printDelim(); printEL();
//    Npulse = (int) Npulse;
//    printDouble(Npulse); printDelim(); printEL();
//    printInt(Npulse); printDelim(); printEL();
    
//    printString("Tpulse*1000 is "); print_val_double(Tpulse*1000);
//    printString("Fs is "); print_val_double(Fs);
//    printDouble(Tpulse * Fs); printDelim(); printEL();
//    printInt(Tpulse * Fs); printDelim(); printEL();
//    printDouble(160000*0.00025); printDelim(); printEL();
//    printInt(160000*0.00025); printDelim(); printEL();
// ======================= TRUNCATOIN ERROR ====================================
    
    // Set waveform to zero
    memset(waveform, 0, sizeof (waveform));
    memset(waveform_w, 0, sizeof (waveform_w));

    if(WAVEFORM==SINE)
    {
        generate_sine(waveform, fc, Fs, Tpulse, NFFT);
        generate_sine(waveform_w, fc, Fs, Tpulse, NFFT);
    }
    else if(WAVEFORM==CHIRP)
    {
        int f1=fc-get_bandwidth()/2;
        int f2=fc+get_bandwidth()/2;
        generate_chirp(waveform,f1,f2,Fs,Tpulse,NFFT);
        generate_chirp(waveform_w,f1,f2,Fs,Tpulse,NFFT);   
    }

//    Apply window function to pulse train
//    /*windowing(Npulse, (const double*) waveform,
//            WINDOW_BLACKMAN, 1, waveform_w);
//    */

//    Get arrays holding input time signal, and output frequency spectrum 
//    of the reference signal
    int32c* refsig_time = get_refsig_time();
//    int32c* refsig_freq = get_refsig_freq();

    // Reformat to double precision floating-point number multiplied by 
    // an exponent of 2 (for the format used in the fft function)
    // generate 34 samples for the reference signal, because waveform_w is initialised to be zeros
    // the following elements are zeros in the array
    for (i = 0; i < NFFT; i++) 
    {
        refsig_time[i].re = (int32_t)ldexp(waveform_w[i], INT_EXPONENT); //INT_EXPONENT
        refsig_time[i].im = 0;     
    }

    int dc_factor = get_dc_factor();
    int Nref_passband = Npulse;  

    // maybe because the timeout restriction on the send_pulse operation, it 
    // is not possible to have the downconversion simultaneous when autoping is on

    int Nref_baseband = Nref_passband / dc_factor + (int)(Nref_passband % dc_factor != 0);

    print_val("Nref_baseband", Nref_baseband);
    print_val("Nref_passband", Nref_passband);

    set_Nref_baseband(Nref_baseband);
    set_Nref_passband(Nref_passband);

//    print_array_int32c(refsig_time, "REFSIG_TIME", Nref_passband);

    int32c refsig_dc_time[Nref_baseband];
    ini_array_int32c(refsig_dc_time, Nref_baseband);

    downconvert_refsig_time(refsig_dc_time, Nref_passband);

    ini_array_int32c(refsig_time, NFFT);
    overwrite_array_int32c(refsig_time, refsig_dc_time, Nref_baseband);

//    printString("setup downconversion is fine!"); printEL();

////     Set timeout according to max range
//    set_timeout(timeout);
//
////     Calculate a minimum threshold
//    minthresh=(int32_t)(pow(2,INT_EXPONENT)*minthresh_frac);
}

void downconvert_refsig_time(int32c *refsig_dc_time, int Nref_passband)
{
    int fc = get_frequency();
    int B = get_bandwidth();
    int Nbpad = get_Nbpad_dcfilt();
    int Nfwpad = get_Nfwpad_dcfilt();
    int dc_factor = get_dc_factor();
    int Nref_baseband = get_Nref_baseband();
    
    int Ncpy = Nref_passband + Nbpad + Nfwpad;

    int32c *refsig_time = get_refsig_time();
    int16c refsig_time_16c[Nref_passband];  /// intermediate buffer for storing refsig_time
    int16c refsig_padded[Ncpy];

    convert32c_to_16c(refsig_time_16c, refsig_time, Nref_passband);
    pad_refsig_time(refsig_padded, refsig_time_16c, Nref_passband, Nbpad, Nfwpad); // padd zeros before and after the main buffer

//    print_array_int16c(refsig_padded, "REFSIG_PAD", Ncpy);

//    print_val("Npassband is ", Nref_passband);
//    print_val("Ndc_baseband is ", Nref_baseband);

// ========================= DOWNCONVERSION SECTION ============================

//    Downconvert all values,, first step is basebanding by shifting the entire spectrum
    int32c dc_vals[Ncpy];
    int16c *dc_exponential = get_dc_exponential();
    mult_complex_arr(dc_vals,refsig_padded,dc_exponential,Ncpy);

//    Rescale previous operation (needed in order to avoid overflow)
    rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3

//    Low pass filter (on pre&post padded buffer slice)
    int Ntaps = get_Ntaps();
    int32c* lpfilt = get_lpfilt();
//    print_val("Ntaps is ", Ntaps);

    ini_array_int32c(refsig_time, NFFT);
    // low pass filtering the basebanded signals
    conv_prepadded_32c(refsig_time,Nref_passband,dc_vals,lpfilt,Ntaps);

//     Resample 
    downsample_overwrite_int32c(refsig_time,Fs,B,Nref_passband);

    rescale_complex2max_int32c(refsig_time,Nref_baseband,RESCALE_FXP);

    overwrite_array_int32c(refsig_dc_time, refsig_time, Nref_baseband);
//    print_array_int32c(refsig_time, "REFSIG_DC_TIME", Nref_baseband);
//    print_array_int32c(refsig_time, "REFSIG_TIME", NFFT);
}

void ini_array_int32c(int32c *arr, int Nlen)
{
    int i;
    for(i = 0;i<Nlen;i++)
    {
        arr[i].re = 0;
        arr[i].im = 0;
    }
}

void overwrite_array_int32c(int32c *arr1, int32c *arr2, int Nlen)
{
    int i;
    for(i = 0;i<Nlen;i++)
        arr1[i] = arr2[i];
}

void print_array_int32c(int32c *p, char *s, int N)
{
    print_start_mark(s);
    int i;
    for(i = 0;i<N;i++)
    {
        printLongInt(p[i].re); printDelim(); printEL();
        printLongInt(p[i].im); printDelim(); printEL();
    }
    print_end_mark(s);
}

void print_array_int16c(int16c *p, char *s, int N)
{
    print_start_mark(s);
    int i;
    for(i = 0;i<N;i++)
    {
        printInt(p[i].re); printDelim(); printEL();
        printInt(p[i].im); printDelim(); printEL();
    }
    print_end_mark(s);
}

void print_array_int32_t(int32_t *p, char *s, int N)
{
    print_start_mark(s);
    int i;
    for(i = 0;i<N;i++)
    {
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark(s);
}

void print_refsig_time(void)
{
    int i;
    int32c *p = get_refsig_time();
    print_start_mark("REFSIG_TIME");
    for(i = 0;i<NFFT;i++)
    {
        printLongInt(p[i].re); printDelim(); printEL();
        printLongInt(p[i].im); printDelim(); printEL();
    }
    print_end_mark("REFSIG_TIME");
}

void pad_refsig_time(int16c *refsig_padded, int16c *refsig_time, int Nref_passband, int Nbpad, int Nfwpad)
{
    int Ncpy = Nref_passband + Nbpad + Nfwpad;
    int i;
    for(i = 0;i<Ncpy;i++)
    {
        if (i<Nbpad)
        {
            refsig_padded[i].re = 0;
            refsig_padded[i].im = 0;
        }
        else if (i>=Nbpad && i<Nref_passband + Nbpad)
        {
            refsig_padded[i].re = refsig_time[i-Nbpad].re;
            refsig_padded[i].im = refsig_time[i-Nbpad].im;  
        }
            else
            {
                refsig_padded[i].re = 0;
                refsig_padded[i].im = 0;
            }
    }
//    print_start_mark("PAD");
//    for(i = 0;i<Ncpy;i++)
//    {
//        printInt(refsig_padded[i].re); printDelim(); printEL();
//        printInt(refsig_padded[i].im); printDelim(); printEL();
//    }
//    print_end_mark("PAD");
//    printString("Padding is okay"); printEL();
}

void flip_array_int32c(int32c *p, int Nbuffer)
{
    int i = 0;
    int j = Nbuffer - 1;
    int32c temp;
    while(i<j)
    {
        temp = p[i];
        p[i] = p[j];
        p[j] = temp;
        i++; j--;
    }
}

void fft32(int32c * signal_freq, int32c * signal_time)
{
    int32c* scratch32 = get_scratch32();  
    int32c* twiddles32 = get_twiddles32(); 
    // Calculate frequency response of reference signal
    DSP_TransformFFT32(signal_freq, signal_time, twiddles32, scratch32, LOG2NFFT);
}

void fft32_dc(int32c * signal_freq, int32c * signal_time)
{
    int32c* scratch32_dc = get_scratch32_dc();  
    int32c* twiddles32_dc = get_twiddles32_dc(); 
    // Calculate frequency response of reference signal
    DSP_TransformFFT32(signal_freq, signal_time, twiddles32_dc, scratch32_dc, LOG2NFFT_DC);
}

void matched_filter_dc_setup(void)
{
//    int fc = get_frequency();
    int fc = 47000;
    int B = get_bandwidth();
    setup_downconversion(fc, B);
    setup_MatchedFilter(6.0,SINE);
}

void test_load_file_matched_filtering(void)
{
    ini_everything();
//    matched_filter_dc_setup();
    load_file_matched_filter_corr();
}

void convert16c_to_32c(int32c *out, int16c *in, int Nlen)
{
    int i;
    for(i = 0;i<Nlen;i++)
    {
        out[i].re = in[i].re;
        out[i].im = in[i].im;
    }
}

void convert32c_to_16c(int16c *out, int32c *in, int Nlen)
{ // scale down is required
    int i;
    for(i = 0;i<Nlen;i++)
    {
        out[i].re = in[i].re;
        out[i].im = in[i].im;
    }
}

void load_file_matched_filter_corr(void)
{
    int Npulse = get_Npulse();
    int dc_factor = get_dc_factor();
    int Nref_passband = get_Nref_passband();
    int Nref_baseband = get_Nref_baseband();

    print_val("Npulse is ", Npulse);
    print_val("dc_factor", dc_factor);
    print_val("Nref_passband", Nref_passband);
    print_val("Nref_baseband", Nref_baseband);
    
    int32c refsig_dc_time[Nref_baseband];

    int32c *refsig_time = get_refsig_time();
    overwrite_array_int32c(refsig_dc_time, refsig_time, Nref_baseband); // to copy from the global variables

//    print_array_int32c(refsig_dc_time, "REFSIG_DC_TIME", Nref_baseband);
    
    cmnd_load_file();

    int i;
    int N_input = (int)get_load_num();  
//    int Nframe = get_Nframe() + 2 * Npulse;
    int Nframe = get_Nframe();
    int Nbaseband = (int) Nframe / dc_factor;
    int Npassband = Nbaseband * dc_factor;
    int Nframes = (int) N_input / Npassband;
    int Nfwpad_dcfilt = get_Nfwpad_dcfilt();
    Nframes++;

    print_val("N_input", N_input);
    print_val("Nframe", Nframe);
    print_val("Nbaseband", Nbaseband);
    print_val("Npassband", Npassband);
    print_val("Nframes", Nframes);
    print_val("Nfwpad_dcfilt", Nfwpad_dcfilt);
    
    
    int32_t abs_corr_time[NFFT_DC];

    while(Nframes--)
    {
        for (i=0;i<Npassband+Nfwpad_dcfilt;i++)
            insert_val_into_adcbuffer((int16_t)read_from_storage());
//        print_adc_buffer_Nframe(Nframe);
        
        downconvert_adc_buffer(Npassband);
//        downconvert_corr_fixed_thresholding_adc_buffer_dc(Npassband);

        int Ndc_baseband = get_Ndc_baseband();
        int32c *dc_vals_lpfilt = get_dc_vals_lpfilt();

        print_val("Ndc_passband is ", Npassband);
        print_val("Ndc_baseband is ", Ndc_baseband);

        print_array_int32c(dc_vals_lpfilt, "DC_VALS_OUT", Ndc_baseband);
        
        int32c signal_dc_time[Ndc_baseband];
        ini_array_int32c(signal_dc_time, Ndc_baseband);
        overwrite_array_int32c(signal_dc_time, dc_vals_lpfilt, Ndc_baseband);

        print_array_int32c(signal_dc_time, "REFSIG_DC_TIME", NFFT_DC);
//        print_array_int32c(refsig_dc_time, "REFSIG_TIME", Nref_baseband);

        // Create correlation holder   
        int32c* ctime=get_ctime();

        // Correlate using the convolution formula to speed up the calculation
        conv_prepadded_32c(ctime,NFFT_DC,signal_dc_time,refsig_dc_time,Nref_baseband);
        print_array_int32c(ctime, "CORR_TIME", NFFT_DC);
        
        rescale_complex2max_int32c(ctime,NFFT_DC,RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);
//        print_array_int32c(ctime, "CORR_TIME", NFFT_DC);

        print_array_int32_t(abs_corr_time, "ABS_CORR_TIME", NFFT_DC);
    }
}

void test_matched_filtering_with_fixed_threshold_dc(int Nframes)
{
    matched_filter_dc_setup();
//    print_val("fixed_threshold is ", get_fixed_threshold_dc());
    matched_filter_fixed_threshold_dc_detector(Nframes);
    
//    load_file_matched_filter_corr();
//    load_file_matched_filter_thresholding();
}

void test_matched_filtering_with_adapted_threshold_dc(int Nframes)
{
//    matched_filter_dc_setup();
//    ini_threshold_buffer_dc_corr(390);
    printString("NOISE LEVEL BUFFER IS SET UP SUCCESSFULLY!"); printEL();
    
//    print_val("fixed_threshold is ", get_fixed_threshold_dc());
    
    matched_filter_with_adapted_threshold_dc_detector(Nframes);
    
//    load_file_matched_filter_corr();
//    load_file_matched_filter_thresholding();
}

void matched_filter_correlation()
{
    int Ndc_baseband = get_Ndc_baseband();
    int32c *dc_vals_lpfilt = get_dc_vals_lpfilt();

    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();

    int32c * ctime = get_ctime();
    // Correlate using the convolution formula to speed up the calculation
    conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
    rescale_complex2max_int32c(ctime,NFFT_DC,RESCALE_FXP);
}

void matched_filter_fixed_threshold_dc_detector(int Nframes)
{
    long int Fs = get_Fs();
    print_val("fs is ", Fs);
    start_sampling(Fs,.5);
    int Npulse = get_Npulse();
    int dc_factor = get_dc_factor();

    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();
    
    int32c *dc_vals_lpfilt;
    int32c *ctime = get_ctime();

    int Ndc_baseband = NFFT_DC + Nref_baseband;
    int Ndc_passband = Ndc_baseband * dc_factor;

    int Nfwpad = get_Nfwpad_dcfilt();
    int numOfExceedance = 0;
    Time t1;
    Time t2;
    int i;
    int threshold_dc = get_fixed_threshold_dc();

    print_start_mark("VALCNT");
    while(Nframes--)
    {
        int exceedance = 0;
//        t1.clk = get_time_clk();
        acquire_samples(Ndc_passband + Nfwpad);
//        print_adc_buffer_valcnt(); //------- test mark
        print_val_int(get_adc_buffer_valcnt());
        
//        numOfExceedance += fixed_thresholding(Ndc_passband);
        
//        downconvert_adc_buffer(Ndc_passband);
        downconvert_corr_fixed_thresholding(Ndc_passband);

        print_val_int(get_adc_buffer_valcnt());
//        print_array_int32c(ctime, "CORR_TIME", NFFT_DC);
//        print_adc_buffer_valcnt(); //------- test mark

//        print_array_int32c(corr_time, "CORR_TIME", NFFT_DC);

        // THREHOLDS FROM DIFFERENT METHODS
//        numOfExceedance += fixed_thresholding_dc(corr_time, NFFT_DC); // used for fixed threshold
//        numOfExceedance += adapted_thresholding(Nframe); // used for adapted threshold
        
//        print_val("numof Exceednace is ", numOfExceedance);

//        t2.clk = get_time_clk();
//        print_adc_buffer_valcnt(); //------- test mark
//        printString("the time it takes for this operation is "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();
    }
    print_end_mark("VALCNT");

    transmit_storage_vector();
    transmit_storage_vector_test();
    print_val("# of exceedances is ", numOfExceedance);
    stop_sampling();
}

void matched_filter_with_adapted_threshold_dc_detector(int Nframes)
{
    long int Fs = get_Fs();
    start_sampling(Fs,.5);

    int Npulse = get_Npulse();

//    int Nframe = get_Nframe() + 2 * Npulse;
    int Nframe = get_Nframe();
    int dc_factor = get_dc_factor();

    int Ndc_baseband = Nframe / dc_factor;
    int Ndc_passband = Ndc_baseband * dc_factor;

//    print_val("Npulse is ", Npulse);
//    print_val("dc_factor is ", dc_factor);
//    print_val("Nframe is ", Nframe);
//    print_val("Ndc_passband is ", Ndc_passband);
//    print_val("Ndc_baseband is ", Ndc_baseband);
//    print_val("Nref_baseband is ", Nref_baseband);

    int Nfwpad = get_Nfwpad_dcfilt();
    int numOfExceedance = 0;
    Time t1;
    Time t2;
    int i;

    print_start_mark("VALCNT");
    while(Nframes--)
    {
//        int exceedance = 0;
//        t1.clk = get_time_clk();
        acquire_samples(Ndc_passband + Nfwpad);
//        print_adc_buffer_valcnt(); //------- test mark
        print_val_int(get_adc_buffer_valcnt());

        numOfExceedance += downconvert_corr_adapted_thresholding(Ndc_passband);

        print_val_int(get_adc_buffer_valcnt());
//        t2.clk = get_time_clk();
//        print_adc_buffer_valcnt(); //------- test mark
//        printString("the time it takes for this operation is "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();
    }
    print_end_mark("VALCNT");

    transmit_storage_vector();
    transmit_storage_vector_test();
    print_threshold_buffer();
    print_val("# of exceedances is ", numOfExceedance);

    stop_sampling();
}

void mf_detector_setup(long int fc)
{
    int B = get_bandwidth();
    set_frequency(fc);
    setup_downconversion(fc, B);
    setup_MatchedFilter(6.0,SINE);
    ini_threshold_buffer_dc_corr(390); // initialise the threshold buffer
}

void test_mf_detector(int transducer)
{   
    long int Fs = get_Fs();
    start_sampling(Fs,.5);

    int Ndc_passband = get_Ndc_passband();
    int Ndc_baseband = get_Ndc_baseband();

//    print_val("Ndc_passband is ", Ndc_passband);
//    print_val("Ndc_baseband is ", Ndc_baseband);

    time_current.clk = get_time_clk();
    print_val("current time is ", time_current.clk);
    
    int Nfwpad = get_Nfwpad_dcfilt();
    int exceedance = FALSE;
    int i = 0;

//    print_start_mark("VALCNT");
    while(TRUE&&i++<100)
    {
        acquire_samples(Ndc_passband + Nfwpad);
//        print_val_int(get_adc_buffer_valcnt());
        exceedance = downconvert_corr_adapted_thresholding(Ndc_passband);
//        print_val_int(get_adc_buffer_valcnt());
//        print_val_int(i);
    }
//    print_val_int(i);
//    print_end_mark("VALCNT");

    if(exceedance)
    {
        set_exceedance(TRUE);
        printString("Exceedance happened"); printEL();
    }
    time_current.clk = get_time_clk();
    print_val("current time is ", time_current.clk);

    print_threshold_buffer();       // all thresholds in the buffer
    transmit_storage_vector();          // all correlated dc values from the received buffer
    transmit_storage_vector_test();  // all thresholds

    stop_sampling();

}

void test_signal_generation(void)
{
    int i;
    int np = get_np(); // Number of pulses
    int Fs = get_Fs();
    //double dt = 1 / (double) Fs;
    int fc = get_frequency(); 
    print_val("np is ", np);
    print_val("Fs is ", Fs);
    print_val("fc is ", fc);
    
    double Tp = 1 / (double)fc;
    Tpulse = Tp*np; // One wave period
    Npulse = Tpulse*Fs; // Total number of points in the pulse
    
    print_val("Tpulse is ", Tpulse);
    print_val("Npulse is ", Npulse);
    int f1, f2;
    // Set waveform to zero
    memset(waveform, 0, sizeof (waveform));
    memset(waveform_w, 0, sizeof (waveform_w));
    
    int WAVEFORM = CHIRP;
    
    if(WAVEFORM==SINE){
        generate_sine(waveform, fc, Fs, Tpulse, NFFT);
        generate_sine(waveform_w, fc, Fs, Tpulse, NFFT);
    }
    else if(WAVEFORM==CHIRP){
        f1=fc-get_bandwidth()/2;
        f2=fc+get_bandwidth()/2;
        generate_chirp(waveform,f1,f2,Fs,Tpulse,NFFT);
        generate_chirp(waveform_w,f1,f2,Fs,Tpulse,NFFT);   
    }
    
    print_val("f1 is ", f1);
    print_val("f2 is ", f2);

    print_start_mark("WAVEFORM");
    for (i = 0;i<NFFT;i++)
    {
        printDouble(waveform[i]); printDelim(); printEL();
    }
    print_end_mark("WAVEFORM");

    print_start_mark("WAVEFORM_W");
    for (i = 0;i<NFFT;i++)
    {
        printDouble(waveform_w[i]); printDelim(); printEL();
    }
    print_end_mark("WAVEFORM_W");
}

void generate_sine(double* sine, int frequency, int Fs, double T, int Nlen){
    double Np = T*Fs; // Total number of points in the pulse
    //printString("generating square np="); printInt((int) (T*frequency)); printEL();
    double dt = 1 / (double) Fs;
    int i = 0;
    do {//generate echo template
        sine[i] = sin((double) 2 * M_PI * frequency * i * dt);
//        sine[i] = sin((double) 2 * M_PI * frequency * i/Fs);
        i++;
    } while (i <= (int)Np && i < Nlen);   
}

void generate_chirp(double* chirp, int f1, int f2, int Fs, double T, int Nlen){
        //double w1, double w2, double A, double M, double time){
   //res=A*cos(w1*time+(w2-w1)*time*time/(2*M));
   
    double Np = T*Fs; // Total number of points in the pulse
    //printString("generating chirp np="); printInt((int) (T*(f2+f1)/2)); printEL();

    double dt = 1 / (double) Fs;
    double w1=f1*2*M_PI; double w2=f2*2*M_PI;
    double time=0;
    int i = 0;
    do {//generate echo template
        time=i*dt;
        chirp[i] = cos(w1*time+(w2-w1)*time*time/(2*T));
                //sin((double) 2 * M_PI * frequency * i * dt);
        i++;
    } while (i <= (int)Np && i < Nlen);  
}

// Correlate
void correlate(int32c* result, int32c* vref, int32c* vcomp, int cstart, int cstop){
    int j; 
    //for (j=cstart;j<cstop;j++){
    for (j=0;j<NFFT-1;j++){
        result[j].re = vref[j].re*vcomp[j].re+vref[j].im*vcomp[j].im;
        result[j].im = -vref[j].re*vcomp[j].im+vref[j].im*vcomp[j].re; 
    }
}

// Correlate
void correlate_dc(int32c* result, int32c* vref, int32c* vcomp, int cstart, int cstop){
    int j; 
    //for (j=cstart;j<cstop;j++){
    for (j=0;j<NFFT_DC-1;j++){
        result[j].re = vref[j].re*vcomp[j].re+vref[j].im*vcomp[j].im;
        result[j].im = -vref[j].re*vcomp[j].im+vref[j].im*vcomp[j].re; 
    }
}

// Getters and setters for sonar variables

void set_consecutive_hits(int h) {
    consecutive_hits = h;
}

int get_consecutive_hits(void) {
    return consecutive_hits;
}

void set_consecutive_misses(int m) {
    consecutive_misses = m;
}

int get_consecutive_misses(void) {
    return consecutive_misses;
}

void set_soundspeed(int s) {
    soundspeed = s;
}

float get_soundspeed(void) {
    return soundspeed;
}

/*float get_samplingrate(void) {
    return samplingrate;
}*/

int get_trig_num(void) {
    return trig_num;
}

void set_trig_num(int tr) {
    trig_num = tr;
}

/*int get_thresh_weight(void) {
    return thresh_weight;
}

void set_thresh_weight(int tr) {
    thresh_weight = tr;
}*/

double get_deadtime(void) {
    return deadtime;
}

void set_deadtime(double d) {
    deadtime = d;
}

void set_t_samplstart(int tspl) {
    t_samplstart = tspl;
}

int get_t_samplstart(void) {
    return t_samplstart;
}

void set_Fs(int f) {
    Fs = f;
}

int get_Fs(void) {
    return Fs;
}

void set_timeout(double t){
    // Set timeout in seconds, converts to samples for
    // internal use
    timeout=(int) (Fs*t);    
}

double get_timeout(void){
    return ((double)timeout)/get_Fs();
}

// Get pointer to pre stored waveform
double* get_waveform(void){
    return waveform;
}

int get_wave_select(void){
    return wave_select;
}

void set_wave_select(int ws){
    wave_select=ws;
}

int get_Npulse(void) {
    return Npulse;
}

void set_Npulse(double nPulse)
{
    Npulse = nPulse;
}

int get_Nref_passband()
{
    return Nref_passband;
}

void set_Nref_passband(int Nvals)
{
    Nref_passband = Nvals;
}

int get_Nref_baseband()
{
    return Nref_baseband;
}

void set_Nref_baseband(int Nvals)
{
    Nref_baseband = Nvals;
}

int get_index_dc_exceedance(void)
{
    return index_dc_exceedance;
}

void set_index_dc_exceedance(int val)
{
    index_dc_exceedance = val;
}

Time get_time_arrival(void)
{
    return time_arrival;
}

void set_time_arrival(Time time)
{
    time_arrival = time;
}

Time get_time_current(void)
{
    return time_current;
}

void set_time_current(Time time)
{
    time_current = time;
}

Time get_fixed_time_delay(void)
{
    return fixed_time_delay;
}

void set_fixed_time_delay(Time time)
{
    fixed_time_delay = time;
}