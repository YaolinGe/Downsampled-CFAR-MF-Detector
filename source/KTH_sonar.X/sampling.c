#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include "sampling.h"
#include "delay.h"
#include "SONARconfig.h"
#include "memory.h"
#include "modes.h"
#include <math.h>
#include "downlink.h"
#include "messages.h"
#include "time.h"
#include "sonar_math.h"
#include "commands.h"

// ================ debug can be deleted ====================
#include "sonar.h"
#include "detection.h"
#include "listen.h"
#include "pulse.h"

// Rescaling factors in fixed point implementation
#define RESCALE_DC_EXPO 511 // Scale for downconversion exponential (2^9-1) 
#define RESCALE_FXP 1023 // Scale for fxp operations (2^10-1) // deduct 1 to make sure no overflow happened
//#define SCALE_OVERFLOW 32768 // used to scale the downconversion

// Sampling variables
char new_sampl=FALSE;
long int Fs=DEFAULT_SAMPLING_FREQUENCY; // Sampling rate 

// Buffers ----------------------
static int adc_bufferlen = 2*NFFT;
static int adc_bufferlen_dc = 2*NFFT;

int16_t adc_buffer[2*NFFT];
int16_t adc_buffer_cpy[2*NFFT]; // Used for copying out values from the buffer
int16c adc_buffer_cpyc[2*NFFT]; // Used for copying out values from the buffer
volatile int index_adcbuffer_nextsmpl = 0; // Write new samples to this index
volatile int index_adcbuffer_oldest = 0; // Read from this index
int adc_buffer_valcnt=0;
int16c test[NFFT];
// Time tracker of the oldest value in the buffer sequence
// that was latest extracted with unload_adc_buffer(), and latest
// adc sample
Time t_buffer;
Time t_latest_sample;
Time t0_alg;
Time timeout_alg;
Time timeout_sampl;
int timeout_cnt = 0;
#define TIMEOUT_FRAMES_BEACON 300
#define TIMEOUT_FRAMES_TRANSMITTER 300
#define DELAY_SAMPLES 10
#define CALIBRATED_DELAY 0      // sec
#define COMPENSATED_TIME 8036762 // compensated time
//#define CALIBRATED_DELAY 900000  // delay in ns


//int t_delay = 2;      //[ms], fixed delay
//int32_t fixed_delay = CALIBRATED_DELAY;    // sec
int32_t fixed_delay = 1;    // sec
//int32_t fixed_delay = 693000000; // delay in clk
//int32_t fixed_delay = sec2clk(CALIBRATED_DELAY / 1000000);

// Filters --------------------------------------
int Nbpad_dcfilt; // Number backward padding taps for lp filter
int Nfwpad_dcfilt; // Number forward padding taps for lp filter
int Ntaps_dcfilt;
// Lowpass input filter -- 5kHz
int Ntaps_5kHz=7; // but actually it is the order of 6, since its z-transform highest order is z^-6
int32c lpfilt[7]; // Must be length of longest implemented filter
int16_t lpfilt_5kHz_coeffs[]=//{100,437,915,1151,915,437,100};
                //{-72,201,1294,2048,1294,201,-72};
//    {
        {178,778,1628,2048,1628,778,178};
//        {178,778,1628,2048,1628,778,178}
//    };

// Buffer for filter output
int filter_output_len=(2*NFFT);
int32c filter_output_buffer[2*NFFT]; // Needs to be 32bit, might be large values
int buffer_cpy_clip_flag=0; // Might be used later
int ref_signal_cpy_clip_flag=0; // Might be used later
int buffer_overextract_flag=0;

long int dc_Fs; // Downconverted sampling frequency 
int dc_factor; // Downsampled output has 1/dc_factor number of samples
int Ndc_baseband=0; // Track how many equiv. baseband samples have been buffered in lowpass
int Ndc_passband=0; // Track how many lowpass values have been converted

// Buffer for downconversion exponential
int dc_vecs_len=2*NFFT;
int16c dc_exponential[2*NFFT];
// Buffer for downconversion output
int32c dc_vals[2*NFFT];
// Buffer for lp filter output
int32c dc_vals_lpfilt[2*NFFT];
int dc_index[2*NFFT];
// Buffer for resampled output from lp filter

// Buffer for resampled & lp filtered output 
int16c adc_buffer_dc[2*NFFT_DC];

#define RAW_DATA_BUFFER_LEN 20000
int raw_data_buffer_len = RAW_DATA_BUFFER_LEN;
int index_raw_data_buffer=0;
int16_t raw_data[RAW_DATA_BUFFER_LEN];
int raw_data_buffer_valcnt=0;
// Old - will be removed ----------------------
// for feeding the adc values into DSP
//int32c buff_unf[NFFT]; // Holds time inverted unfolded adc buffer
//int32c buff_unf_freq[NFFT]; // Holds frequency version of time inverted unfolded adc buffer
// -------

// Noise variance buffers ----------------------
// NVAR_BUFFER_LEN=(2^31-1)/(2*RESCALE_FXP^2), largest possible sum
// without risk of overflow in a sum over variance variables
#define NOISE_BUFFER_LEN 1026    // for approximately 5 secs time duration
int threshold_buffer_len=NOISE_BUFFER_LEN;
int index_threshold_buffer=0;
int32_t threshold_buffer[NOISE_BUFFER_LEN];
int32_t noise_level = 0;
int threshold_buffer_valcnt=0;
int32_t threshold_buffer_old_avg = 0;
#define MIN_THRESHOLD 2000// minimum threshold allowed for the noise_level
#define MIN_THRESHOLD_DC 20// minimum threshold allowed for the dc noise_level
#define LIFT_FACTOR_BEACON 5 // to lift up the correlation result to avoid zero threshold
#define LIFT_FACTOR_TRANSMITTER 5 // to lift up the correlation result to avoid zero threshold
#define MICRO_FACTOR 1000000    // scale up factor for delaymicro()

int Nframe = NFFT;      // length of Nframe
int Nframe_dc = NFFT_DC;      // length of Nframe
int exceedance = 0; // exceedance of the detection

//debug ----------------------Yaolin ------ can be deleted ---------------------

//end of debug ----------------------Yaolin ------ can be deleted --------------

void __attribute__((interrupt(IPL2AUTO), vector(_TIMER_2_VECTOR)))
Timer2Handler(void);    
//void __attribute__((interrupt(IPL2AUTO), vector(_ADC_DATA18_VECTOR)))
//ADC18InterruptHandler(void);
void Timer2Handler(void) {

    // Debug mode, sample from storage
    if (get_Mode_debug().state == TRUE) {
        insert_val_into_adcbuffer((int16_t)read_from_storage());
        t_latest_sample.clk=get_time_clk(); // Dummy update for testing
    // Normal mode, sample from ADC    
    } else {
        // Wait for adc to finish
        ADCCON3bits.GSWTRG = 1; // Trigger a conversion
        while (ADCDSTAT1bits.ARDY18 == 0);
        t_latest_sample.clk=get_time_clk(); // Time of the latest sample
        insert_val_into_adcbuffer((int16_t)ADCDATA18);
    }
    IFS0bits.T2IF = 0;  // Reset interrupt
}

void debug_t_buffer(void)
{
    int32_t t0, t1;
    TMR4 = 0;
    long int Fs = get_Fs();
    print_val("Fs", Fs);
    int Nframes = 10;
    int Nframe = 2048;
    
    t0 = get_time_clk();
    while(Nframes--)
    {
        acquire_samples(Nframe);
        int16_t *p = unload_adc_buffer(Nframe, 0, 0);
    }
    t0 = get_time_clk();

    
}

void debug_overflow(void)
{
    TMR4 = 0;
    int32_t t;
    Time t1, t2;
//    t1.clk = get_time_clk();
    t1.clk = 0;
    t2.clk = TMR4;
//    t = get_time_clk();
    while(t2.clk!=0)
    {
        t1.clk = t2.clk;
//        printString("#"); printEL();
        printLongInt(t2.clk); printEL();
        t2.clk = get_time_clk();
    }
//    t2.clk = get_time_clk();
    printString("the curernt time frame is "); printDouble(clk2sec(t1.clk));printEL();
}

void load_file_matched_filter_thresholding(void)
{
    int Npulse = get_Npulse();
    int dc_factor = get_dc_factor();
    int Nref_passband = Npulse;
    int Nref_baseband = Nref_passband/dc_factor + (int)(Nref_passband % dc_factor != 0);

    print_val("Npulse is ", Npulse);
    print_val("dc_factor", dc_factor);
    print_val("Nref_passband", Nref_passband);
    print_val("Nref_baseband", Nref_baseband);
//    print_val("dc_factor", dc_factor);

    int32c refsig_dc_time[Nref_baseband];
    int32c *refsig_time = get_refsig_time();
    overwrite_array_int32c(refsig_dc_time, refsig_time, Nref_baseband); // to copy from the global variables

    cmnd_load_file();

    int i;
    int N_input = (int)get_load_num();  
//    int Nframe = get_Nframe() + 2 * Npulse;
    int Nframe = get_Nframe();
    int Nbaseband = (int) Nframe / dc_factor;
    int Npassband = Nbaseband * dc_factor;
    int Nframes = (int) N_input / Npassband;
    int Nfwpad_dcfilt = get_Nfwpad_dcfilt();
    int Ncpy = get_Ncpy();
    Nframes++;

    print_val("Nframe is ", Nframe);
    print_val("N_input is ", N_input);
    print_val("Nbaseband is ", Nbaseband);
    print_val("Npassband is ", Npassband);
    print_val("Nframes is ", Nframes);
    print_val("Nfwpad_dcfilt is ", Nfwpad_dcfilt);
    print_val("Ncpy is ", Ncpy);

    int32_t abs_corr_time[NFFT_DC];
    int32c* ctime=get_ctime();
    
    while(Nframes--)
    {
        int temp = get_index_adcbuffer_oldest();
        for (i=0;i<Npassband+Nfwpad_dcfilt;i++)
            insert_val_into_adcbuffer((int16_t)read_from_storage());
        set_index_adcbuffer_oldest(temp);  // avoid the interference 
//        set_index_adcbuffer_nextsmpl(0);
        
//        print_adc_buffer_Nframe(Npassband);

        int16c* cpy = unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        print_array_int16c(cpy, "CPY_BIAS", Ncpy);

        // remove the bias, or the dc component of the signal
        int32_t mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
            cpy[i].re=cpy[i].re-mean;
        print_array_int16c(cpy, "CPY", Ncpy);
        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        print_array_int32c(dc_vals, "DC_VALS", Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        print_array_int32c(dc_vals, "DC_VALS_RESCALE", Ncpy);

        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT", Ndc_passband);

        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT_DOWNSAMPLED", Ndc_baseband);

        // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT_DOWNSAMPLED_RESCALE", Ndc_baseband);

        // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

        print_array_int32c(ctime, "CORR_TIME", NFFT_DC);
        print_array_int32_t(abs_corr_time, "ABS_CORR_TIME", NFFT_DC);
        // THREHOLDS FROM DIFFERENT METHODS
//        numOfExceedance += fixed_thresholding_dc(corr_time, NFFT_DC); // used for fixed threshold        
    }
}

void debug_clk_speed_tracking(void)
{
    TMR4 = 0;
    Time t1, t2;
    t1.clk = get_time_clk();
    delaymicrosec(1000);
    t2.clk = get_time_clk();
    print_val("delay is ", t2.clk - t1.clk);
    
//    send_pulse(40000, SINE, 0);
    t1.clk = get_time_clk();
    delaymicrosec(1000);
    t2.clk = get_time_clk();
    print_val("delay is ", t2.clk - t1.clk);
    
    t1.clk = get_time_clk();
//    delay_clk();
    t2.clk = get_time_clk();
    print_val("delay is ", t2.clk - t1.clk);
}

void debug_centre_frequency_recalibration(void)
{
    int i = 0;
    int N = 21;
    long int fc;
    
    for(i=0;i<N;i++)
    {
        fc = 30000+i*1000;
//        fc = (i+1)*5000;

        printString("the centre frequency is "); printLongInt(fc); printEL();
        set_frequency(fc);
//        print_val("frequency", get_frequency());
        
        ini_everything();
//        send_pulse(fc, SINE, 0);
        
        Time tlisten;
        Time tpulse;
        tpulse.clk=get_t_pulsestart();
        tlisten.clk=get_time_clk();
        listen_full_storage(); // listen to fulfill the entire storage vector
        printEL();
        printString("Time elapsed is "); printDouble(clk2sec(tlisten.clk-tpulse.clk));printEL();
        delaymilli(100);
    }
}

void debug_start_delay_recalibration(void)
{
    int i = 0;
    int N = 31;
    int start_delay;
    
    for(i=0;i<N;i++)
    {
//        TMR4 = 0;
        ini_everything();
        start_delay = i*50;
//        printString("the start_delay is "); printInt(start_delay); printEL();
        set_startdelay(start_delay);
        print_val("frequeny", get_frequency());
        print_val("start_delay", get_startdelay());
        
//        send_pulse(40000, SINE, 0);
        
        Time tlisten;
        Time tpulse;
        tpulse.clk=get_t_pulsestart();
        tlisten.clk=get_time_clk();
        listen_full_storage(); // listen to fulfill the entire storage vector
//        listen_N_samples(1000);
        printEL();
        printString("Time elapsed is "); printDouble(clk2sec(tlisten.clk-tpulse.clk));printEL();
        delaymilli(3000);
    }
}

void debug_ramping_up_delay_calibration(void)
{
    int i = 0;
    int N = 41;
    int start_delay;
    
    for(i=0;i<N;i++)
    {
//        TMR4 = 0;
        ini_everything();
        start_delay = 6000;
//        printString("the start_delay is "); printInt(start_delay); printEL();
        set_startdelay(start_delay);
        print_val("frequeny", get_frequency());
        print_val("start_delay", get_startdelay());
        
        send_pulse(40000, SINE, 0);
        
        Time tlisten;
        Time tpulse;
        tpulse.clk=get_t_pulsestart();
        tlisten.clk=get_time_clk();
//        listen_full_storage(); // listen to fulfill the entire storage vector
        
        setTransducerToSensing();
    //    printString("set to sensing is fine!"); printEL();
        delaymicrosec(i*10);   // delay to remove the bias from the ADC, or remove the capacitor charging effect
        
        listen_N_samples(1000);
        printEL();
        printString("Time elapsed is "); printDouble(clk2sec(tlisten.clk-tpulse.clk));printEL();
        delaymilli(1000);
    }
}

void debug_beacon(void)
{
    int scale_factor;
    int i;
    int N = 50;
    for(i = 5;i<N;)
    {
        ini_everything();
        set_scale_factor(i);
        i += 5;
        print_val("scale_factor", get_scale_factor());
        beacon();
        transmit_storage_vector();
        transmit_storage_vector_test_N(3*TIMEOUT_FRAMES_BEACON);
        print_threshold_buffer();
    }
}

void debug_transmitter(void)
{
        int i = 0;
        int N = 500;
        print_start_mark("RANGE_EST");
        for(i = 0;i<N;i++)
        {
            transmitter();
            delaymilli(500);
        }
        print_end_mark("RANGE_EST");
}

void test_time_calculation(void)
{
    Time t_begin, t_end;
    t_begin.clk = get_time_clk();
    delaymilli(1000);

    t_end.clk = get_time_clk();
    print_start_mark("DELAY");
    print_val_double( get_time_diff(t_begin, t_end));
    print_end_mark("DELAY");

//    int32_t t1;
//    int32_t t2;
//    int i = 0;
//    int32_t t_total;
//    t1 = get_time_clk();
//    t_total = 0;
//    while(i++<10)
//    {
//        printInt(i); printEL();
//        t2 = get_time_clk();
//        t_total += t2 - t1;
//        print_val("time_diff", t2 - t1);
//        t1 = get_time_clk();
//    }
//    print_val("t-tal", t_total);
//    int Nframe = get_Nframe();
//    ini_everything();
//    cmnd_load_file();

//    transfer_data_from_storage_to_adc_buffer(Nframe);

//    int N = get_load_num();
//    int Nframes = (int) N / Nframe + 1;
//    Time t1, t2, t3;
//    int Fs = get_Fs();
//    int i;

//    while(Nframes--)
//    {
//        int16_t * p = unload_adc_buffer(Nframe, 0, 0);
//        t1 = get_t_buffer();
//        for(i = 0;i<Nframe;i++)
//            if(p[i]>3400)
//                break;
//        t2.clk = sampl2clk(i, Fs);
//        t3.clk = get_time_clk();
//        
//        printString("t_buffer is "); printInt(t1.clk); printString("clk"); printEL();
//        printString("Time of arrival is "); printInt(t2.clk); printString("clk"); printEL();
//        printString("Time of arrival is "); printInt(t3.clk); printString("clk"); printEL();
//
//        printString("t_buffer is "); printDouble(clk2sec(t1.clk)); printString("secs"); printEL();
//        printString("Time of arrival is "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();
//        printString("Time of arrival is "); printDouble(get_time_diff(t1, t3)); printString("secs"); printEL();
//    }
//    print_adc_buffer();

}

void send_pulse(long int fc, long int bandwidth, int waveselect)
{
    set_frequency(fc);
    set_startdelay(CALIBRATED_DELAY*MICRO_FACTOR);
    
    int np = (int) fc / bandwidth;
    set_np(np);

    set_wave_select(waveselect);
    if(get_wave_select()==SINE){
        send_square_pulse(get_np(), get_frequency(), get_voltage());
    } else if(get_wave_select()==CHIRP){
        send_chirp_pulse(get_np(),get_voltage(),get_frequency(),get_bandwidth());
    }
}

void insert_threshold_into_buffer(int32_t val) {
    // Check if rounding the buffer
    if (index_threshold_buffer >= threshold_buffer_len) {
        index_threshold_buffer = 0;
    }
    threshold_buffer[index_threshold_buffer] = val;
    index_threshold_buffer++;

    if(threshold_buffer_valcnt<threshold_buffer_len){
        threshold_buffer_valcnt++; // Track number of inserted values
    }else{
        // Full noise buffer, all is well
    }
}

void insert_raw_data_into_buffer(int16_t val) {
    // Check if rounding the buffer
    if (index_raw_data_buffer >= raw_data_buffer_len) {
        index_raw_data_buffer = 0;
    }
    raw_data[index_raw_data_buffer] = val;
    index_raw_data_buffer++;

    if(raw_data_buffer_valcnt<raw_data_buffer_len){
        raw_data_buffer_valcnt++; // Track number of inserted values
    }else{
        // Full noise buffer, all is well
    }
}

void ini_threshold_buffer(void)
{
    int Nbuffer = get_threshold_buffer_len();
    int Fs = get_Fs();
    int scale = get_scale_factor();
    int32_t noise_level;
    int32_t noise_base_level;
    int32_t threshold;
    int i;
    start_sampling(Fs,.5);
    
    Time t1;
    Time t2;
    t1.clk = get_time_clk();
    
    int Nframe = get_Nframe();
    set_index_threshold_buffer(0);
    for (i = 0;i<Nbuffer;i++)
    {
        acquire_samples(Nframe);
//        print_adc_buffer_valcnt();
        int16_t* p=unload_adc_buffer(Nframe,0,0);
        noise_level = calculate_noise_level(p, Nframe);
        noise_base_level = calculate_noise_base_level(p, Nframe);
        threshold = noise_base_level + scale * noise_level;
        insert_threshold_into_buffer(threshold);
//        print_adc_buffer_valcnt();
    }
    print_val("index of noise level buffer is ", get_index_noise_level_buffer());
    t2.clk = get_time_clk();
    printString("the total calculation takes "); printDouble(clk2sec(t2.clk - t1.clk)); printString("secs");printEL();
    print_threshold_buffer();
    stop_sampling();
}

void ini_threshold_buffer_dc_corr(int Nframes)
{
    int Fs = get_Fs();  // Fs = 160e3
    int scale = get_scale_factor();  // scale is 40, can be changed
    int32_t noise_level;
    int32_t noise_dc_level;
    int32_t threshold_dc;
    int i,j;

    Time t1;
    Time t2;
    t1.clk = get_time_clk();

    int32c *refsig_time = get_refsig_time(); // 4 values
    int Nref_baseband = get_Nref_baseband(); // equal to 4
    int Nref_passband = get_Nref_passband(); // equal to 34

    if(Nref_baseband>1)
        Nframe = get_Nframe() + 2 * Nref_passband;
    else
        Nframe = get_Nframe();

    int dc_factor = get_dc_factor();
    Ndc_baseband = Nframe / dc_factor; 
    Ndc_passband = Ndc_baseband * dc_factor;

    int Nfwpad = get_Nfwpad_dcfilt();
    int Nbpad = get_Nbpad_dcfilt();
    int Ncpy = Nbpad + Ndc_passband + Nfwpad;
    
    print_val("Nframe", Nframe);
    print_val("Ndc_baseband is ", Ndc_baseband);
    print_val("Ndc_passband is ", Ndc_passband);
    print_val("dc_factor is ", dc_factor);
    print_val("Nref_baseband is ", Nref_baseband);
    print_val("Nref_passband ", Nref_passband);
    print_val("Fs is ", get_Fs());
    print_val("Ncpy is ", Ncpy);
    print_val("Nframes is ", Nframes);

    int32c *ctime = get_ctime();
    int32_t abs_corr_time[NFFT_DC] = {0};
//    int32_t abs_dc_vals[Ndc_baseband];
    int32_t mean;

    set_index_threshold_buffer(0);
    start_sampling(Fs,.5);
//    print_start_mark("VALCNT");
    for (i = 0;i<Nframes;i++)
    {
        acquire_samples(Ndc_passband + Nfwpad);
//        print_val_int(get_adc_buffer_valcnt());

        int16c * cpy = unload_adc_buffer_complex(Ndc_passband, Nbpad, Nfwpad);

        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (j=0;j<Ncpy;j++)
        {
            cpy[j].re=cpy[j].re-mean;
//            insert_into_storage(cpy[j].re);
        }

        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
    //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
    //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
    // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
//        calculate_abs_corr_time(abs_dc_vals, dc_vals_lpfilt, Ndc_baseband);

    // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
//        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR*RESCALE_FXP);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);

        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

//        for(j = 0;j<NFFT_DC;j++)
//            insert_into_storage_test(abs_corr_time[j]);

        noise_level = calculate_noise_level_dc(abs_corr_time, NFFT_DC);
        noise_dc_level = calculate_noise_dc_level_dc(abs_corr_time, NFFT_DC);
        threshold_dc = noise_dc_level + scale * noise_level;
        insert_threshold_into_buffer(threshold_dc);
        
//        insert_into_storage_test(threshold_dc);
//        print_val_int(get_adc_buffer_valcnt());
    }
//    print_end_mark("VALCNT");

//    print_val("index of noise level buffer is ", get_index_noise_level_buffer());
    t2.clk = get_time_clk();
    stop_sampling();
    printString("the total calculation takes "); printDouble(clk2sec(t2.clk - t1.clk)); printString("secs");printEL();
}

void ini_threshold_buffer_dc_corr_transmitter_side(int Nframes)
{
    int Fs = get_Fs();  // Fs = 160e3
    int scale = get_scale_factor();  // scale is 40, can be changed
    int32_t noise_level;
    int32_t noise_dc_level;
    int32_t threshold_dc;
    int i,j;

    Time t1;
    Time t2;
    t1.clk = get_time_clk();

    int32c *refsig_time = get_refsig_time(); // 4 values
    int Nref_baseband = get_Nref_baseband(); // equal to 4
    int Nref_passband = get_Nref_passband(); // equal to 34

    if(Nref_baseband>1)
        Nframe = get_Nframe() + 2 * Nref_passband;
    else
        Nframe = get_Nframe();

    int dc_factor = get_dc_factor();
    Ndc_baseband = Nframe / dc_factor; 
    Ndc_passband = Ndc_baseband * dc_factor;

    int Nfwpad = get_Nfwpad_dcfilt();
    int Nbpad = get_Nbpad_dcfilt();
    int Ncpy = Nbpad + Ndc_passband + Nfwpad;

    print_val("Nframe", Nframe);
    print_val("Ndc_baseband is ", Ndc_baseband);
    print_val("Ndc_passband is ", Ndc_passband);
    print_val("dc_factor is ", dc_factor);
    print_val("Nref_baseband is ", Nref_baseband);
    print_val("Nref_passband ", Nref_passband);
    print_val("frequency is ", get_frequency());
    print_val("Ncpy is ", Ncpy);
    print_val("Nframes is ", Nframes);

    int32c *ctime = get_ctime();
    int32_t abs_corr_time[NFFT_DC] = {0};
//    int32_t abs_dc_vals[Ndc_baseband];
    int32_t mean;

    set_index_threshold_buffer(0);
    start_sampling(Fs,.5);
//    print_start_mark("VALCNT");
    for (i = 0;i<Nframes;i++)
    {
        acquire_samples(Ndc_passband + Nfwpad);
//        print_val_int(get_adc_buffer_valcnt());

        int16c * cpy = unload_adc_buffer_complex(Ndc_passband, Nbpad, Nfwpad);
//        print_array_int16c(cpy, "CPY_BIAS", Ncpy);

        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (j=0;j<Ncpy;j++)
        {
            cpy[j].re=cpy[j].re-mean;
//            insert_into_storage(cpy[j].re);
        }

//        print_array_int16c(cpy, "CPY", Ncpy);

        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        
//        print_array_int32c(dc_vals, "DC_VALS", Ncpy);
//        print_dc_component(Ncpy);
        
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
//        print_array_int32c(dc_vals, "DC_VALS_RESCALE", Ncpy);

    //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
//        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT", Ndc_passband);

    //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
//        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT_DC", Ndc_baseband);

    // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
//        calculate_abs_corr_time(abs_dc_vals, dc_vals_lpfilt, Ndc_baseband);
//        print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT_DC_RESCALE", Ndc_baseband);

    // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
//        print_array_int32c(ctime, "CORR_TIME", NFFT_DC);

//        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR*RESCALE_FXP);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_TRANSMITTER*RESCALE_FXP);
//        print_array_int32c(ctime, "CORR_TIME_RESCALE", NFFT_DC);

        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);
//        print_array_int32_t(abs_corr_time, "ABS_CORR_TIME", NFFT_DC);

//        for(j = 0;j<NFFT_DC;j++)
//            insert_into_storage_test(abs_corr_time[j]);

        noise_level = calculate_noise_level_dc(abs_corr_time, NFFT_DC);
//        print_array_int32_t(&noise_level, "NOISE_LEVEL", 1);

        noise_dc_level = calculate_noise_dc_level_dc(abs_corr_time, NFFT_DC);
//        print_array_int32_t(&noise_dc_level, "NOISE_DC_LEVEL", 1);

        threshold_dc = noise_dc_level + scale * noise_level;
//        print_array_int32_t(&threshold_dc, "THRESHOLD_DC", 1);

        insert_threshold_into_buffer(threshold_dc);
        
//        insert_into_storage_test(threshold_dc);
//        print_val_int(get_adc_buffer_valcnt());
    }
//    print_end_mark("VALCNT");

//    print_val("index of noise level buffer is ", get_index_noise_level_buffer());
    t2.clk = get_time_clk();
    stop_sampling();
    printString("the total calculation takes "); printDouble(clk2sec(t2.clk - t1.clk)); printString("secs");printEL();
}

void print_threshold_buffer(void)
{
    int32_t *p = get_threshold_buffer();
    int i;
    int Nbuffer = get_threshold_buffer_len();
    print_start_mark("THRESHOLD");
    for (i = 0;i<Nbuffer;i++)
    {
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("THRESHOLD");
}

void print_raw_data_buffer(void)
{
    int16_t *p = get_raw_data_buffer();
    int i;
    int Nbuffer = get_raw_data_buffer_len();
    print_start_mark("RAW_DATA");
    for (i = 0;i<Nbuffer;i++)
    {
        printInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("RAW_DATA");
}

int32_t * get_threshold_buffer(void)
{
    return threshold_buffer;
}

int16_t * get_raw_data_buffer(void)
{
    return raw_data;
}

int get_threshold_buffer_len(void)
{
    return threshold_buffer_len;
}

int get_raw_data_buffer_len(void)
{
    return raw_data_buffer_len;
}

void calculate_abs_corr_time(int32_t *abs_corr_time, int32c * ctime, int Nframe)
{
    int i;
    for(i = 0;i<Nframe;i++)
        abs_corr_time[i] = (int32_t) sqrt(ctime[i].re * ctime[i].re + ctime[i].im * ctime[i].im);
}

int32_t calculate_noise_level(int16_t *p, int Nframe)
{
    int16_t mean = (int16_t) (sum_16_t(p, Nframe) / Nframe);
    int i;
    int32_t var = 0;
    int16_t tmp;
    for (i = 0;i<Nframe;i++)
    {
        tmp = p[i] - mean;
        var +=  tmp * tmp;
    }
    int32_t noise_level = (int32_t) sqrt(var / Nframe); // sqrt of integer is zero if it is zero, floating number, time is longer;
    return noise_level;
}

int32_t calculate_noise_level_dc(int32_t *p, int Nframe)
{
    int i;
    int32_t mean = calculate_noise_dc_level_dc(p, Nframe);
    int32_t var = 0;
    int32_t tmp;
    for (i = 0;i<Nframe;i++)
    {
        tmp = p[i] - mean;
        var +=  tmp * tmp;
    }
    int32_t noise_level = (int32_t) sqrt(var / Nframe); // sqrt of integer is zero if it is zero, floating number, time is longer;
    return noise_level;
}

void update_threshold_buffer(int16_t *p, int Nframe)
{
    int scale = get_scale_factor();
    int32_t noise_level = calculate_noise_level(p, Nframe);
    int32_t noise_base_level = calculate_noise_base_level(p, Nframe);
    int32_t threshold = noise_base_level + scale * noise_level;
    insert_threshold_into_buffer(threshold);
}

int32_t calculate_threshold_avg(void)
{
    int32_t *p = get_threshold_buffer();
    int Nbuffer = get_threshold_buffer_len();
    int32_t res = 0;
    int i;
    for (i = 0; i<Nbuffer; i++)
        res += p[i];
    return (int32_t) (res / Nbuffer);
}

int32_t calculate_noise_base_level(int16_t *p, int Nframe)
{
    return (int32_t)(sum_16_t(p, Nframe)/Nframe);
}

int32_t calculate_noise_dc_level_dc(int32_t *p, int Nframe)
{
    int i;
    int32_t sum = 0;
    for(i = 0;i<Nframe;i++)
        sum += p[i];

    int32_t mean = (int32_t) sum / Nframe;
    return mean;
}

int32_t get_threshold_avg(int16_t *p, int Nframe)
{
    update_threshold_buffer(p, Nframe);
    return calculate_threshold_avg();
}

void update_threshold_buffer_dc(int32_t *p, int Nframe)
{
    int scale = get_scale_factor();
    int32_t noise_level = calculate_noise_level_dc(p, Nframe);
    int32_t noise_base_level = calculate_noise_dc_level_dc(p, Nframe);
    int32_t threshold = noise_base_level + scale * noise_level;
    insert_threshold_into_buffer(threshold);
}

int32_t calculate_threshold_dc_avg(void)
{
    int32_t *p = get_threshold_buffer();
    int Nval = get_threshold_buffer_valcnt();
//    printString("Nval is "); printInt(Nval); printEL();
    
    int32_t res = 0;
    int i;
    for (i = 0; i<Nval; i++)
        res += p[i];
    return (int32_t) (res / Nval);
}

int32_t get_threshold_dc_avg(int32_t *p, int Nframe)
{
    update_threshold_buffer_dc(p, Nframe);
    return calculate_threshold_dc_avg();
}

int adapted_thresholding(int Nframe)
{
    int i;
    int exceedance = FALSE;
    int numOfExceedance = 0;
    
//    print_adc_buffer_valcnt();
    int16_t *p = unload_adc_buffer(Nframe, 0, 0);
//    print_adc_buffer_valcnt();
    int32_t threshold = get_threshold_avg(p, Nframe);
    if (threshold < MIN_THRESHOLD)
        threshold = MIN_THRESHOLD;
//    print_val("MIN_THRES", MIN_THRESHOLD);
//    print_val("threshold", threshold);

    for (i=0;i<Nframe;i++)
    {
        if (p[i]>=threshold)
        {
            exceedance = TRUE;
            break;
        }
    }
    if (exceedance)
    {
        numOfExceedance++;
        for (i=0;i<Nframe;i++)
            insert_into_storage(p[i]);
    }
    return numOfExceedance;
}

void pre_filter_fixed_threshold_detector(int Nframes)
{
    long int Fs = get_Fs();
    start_sampling(Fs,.5);
    int Nsample=get_adc_buffer_len();
    int Nframe = Nsample / 2;
    int numOfExceedance = 0;
    while(Nframes--)
    {
        acquire_samples(Nframe);
//        print_adc_buffer_valcnt(); //------- test mark
        
        // THREHOLDS FROM DIFFERENT METHODS
        numOfExceedance += fixed_thresholding(Nframe); // used for fixed threshold
//        numOfExceedance += adapted_thresholding(Nframe); // used for adapted threshold
//        print_val("numof Exceednace is ", numOfExceedance);
//        print_adc_buffer_valcnt(); //------- test mark
    }
    transmit_storage_vector();
    print_val("# of exceedances is ", numOfExceedance);
    stop_sampling();
}

void pre_filter_adapted_threshold_detector(int Nframes)
{
    long int Fs = get_Fs();
    int scale = get_scale_factor();
    start_sampling(Fs,.5);
    int Nframe = get_Nframe();
    int numOfExceedance = 0;
    Time t1;
    Time t2;
    while(Nframes--)
    {
//        t1.clk = get_time_clk();
        acquire_samples(Nframe);
        print_adc_buffer_valcnt(); //------- test mark
        
        // THREHOLDS FROM DIFFERENT METHODS
        numOfExceedance += adapted_thresholding(Nframe); // used for adapted threshold
//        print_val("numof Exceednace is ", numOfExceedance);
        
        print_adc_buffer_valcnt(); //------- test mark
//        t2.clk = get_time_clk();
//        printString("Adapted thresholding takes "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();
    }
    transmit_storage_vector();
    print_val("# of exceedances is ", numOfExceedance);
    stop_sampling();
}

int fixed_thresholding(int Nframe)
{
    int32_t fixed_threshold = get_fixed_threshold();
//    print_val("fixed_threshold is ", fixed_threshold);

    int i;
    int exceedance = FALSE;
    int numOfExceedance = 0;
    int16_t *p = unload_adc_buffer(Nframe, 0, 0);
    for (i=0;i<Nframe;i++)
    {
        if (p[i]>=fixed_threshold)
        {
            exceedance = TRUE;
            printString("The exceedance happened ");printInt(exceedance);printEL();
            break;
        }
    }
    if (exceedance)
    {
        numOfExceedance++;
        for (i=0;i<Nframe;i++)
            insert_into_storage(p[i]);
    }
    return numOfExceedance;
}

int fixed_thresholding_dc(int32c * p, int Nframe)
{
    int32_t fixed_threshold_dc = get_fixed_threshold_dc();
//    printString("fixed threshold is "); printInt(fixed_threshold_dc); printEL();
    
    int i;
    int exceedance = FALSE;
    int numOfExceedance = 0;
    for (i=0;i<Nframe;i++)
    {
//        if (abs_32c(p[i])>=fixed_threshold_dc)
        if (abs(p[i].re)>=fixed_threshold_dc || abs(p[i].im)>=fixed_threshold_dc)
        {
            exceedance = TRUE;
            printString("The exceedance happened ");printInt(exceedance);printEL();
            break;
        }
    }
    if (exceedance)
    {
        numOfExceedance++;
        for (i=0;i<Nframe;i++)
        {
            insert_into_storage(p[i].re);
            insert_into_storage(p[i].im);
        }
    }
    return numOfExceedance;
}

// Init functions ------------------------------
void init_sampling_lp_filters(int FILTVAL){
    // Initiate filters
    int i;
    switch(FILTVAL)
    {
        case 1:
            Nbpad_dcfilt=Ntaps_5kHz/2;
//            Nfwpad_dcfilt=(int)( ((float)Ntaps_5kHz)/2 + .5); // Ceil())
            Nfwpad_dcfilt=(int)( ((float)Ntaps_5kHz)/2 + .5)-1; // Ceil())
            Ntaps_dcfilt=Ntaps_5kHz;
            for(i=0;i<Ntaps_5kHz;i++)
            {
                lpfilt[i].re=(int32_t)lpfilt_5kHz_coeffs[i];
//                lpfilt[i].re=(int32_t)lpfilt_5kHz_coeffs[1][i];

                lpfilt[i].im=0;
            }
        // Put other filters here 
        case 2:
            Nbpad_dcfilt=Ntaps_5kHz/2;
//            Nfwpad_dcfilt=(int)( ((float)Ntaps_5kHz)/2 + .5); // Ceil())
            Nfwpad_dcfilt=(int)( ((float)Ntaps_5kHz)/2 + .5)-1; // Ceil())
            Ntaps_dcfilt=Ntaps_5kHz;
            for(i=0;i<Ntaps_5kHz;i++)
            {
//                lpfilt[i].re=(int32_t)lpfilt_5kHz_coeffs[2][i];
                lpfilt[i].im=0;
            }
    }
}

//void init_buffers(void) {
//    int i;
//    for(i=0; i<NFFT; i++){
//        // Initiate buffers to zero
//        buff_unf[i].re=0;
//        buff_unf[i].im=0;
//        buff_unf_freq[i].re=0;
//        buff_unf_freq[i].im=0;
//    }
//}

void setup_downconversion(long int f0, long int B){
    // Select filter here 
    int filtselect=1;
    // Setup filter
    init_sampling_lp_filters(filtselect); // Choose downconversion lp filter 1
    // Downconverted sampling rate
    dc_Fs=B; 
    dc_factor=Fs/dc_Fs;
    if(dc_factor<1){ // Crash protection against misstakes
        dc_Fs=Fs;
        dc_factor=1;
    }
    // Create downconversion exponential
    setup_downconversion_exponential(f0);
}

// ADC downconversion functions ----------------
void setup_downconversion_exponential(long int f)
{
    int n;
    double dt=1/(double)Fs;
    for(n=0;n<dc_vecs_len;n++)
    {
        dc_exponential[n].re=(int16_t)(cos((double)2*M_PI*f*n*dt)*RESCALE_DC_EXPO);
        dc_exponential[n].im=(int16_t)(sin((double)-2*M_PI*f*n*dt)*RESCALE_DC_EXPO);
    }
//    print_start_mark("DC_COMP");
//    for(n=0;n<dc_vecs_len;n++){
//        printInt(dc_exponential[n].re); printDelim(); printEL();
//        printInt(dc_exponential[n].im); printDelim(); printEL();
//    }
//    print_end_mark("DC_COMP");
}

// Function for actually triggering downconversion
void downconvert_adc_buffer(int Nvals)
{
    Ndc_baseband=Nvals/dc_factor;  // Compute number of baseband values 
    Ndc_passband=Ndc_baseband*dc_factor; // Compute number of passband values 

    int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);
    int Ncpy=Nbpad_dcfilt+Ndc_passband+Nfwpad_dcfilt;
    int i;

//    int threshold = get_fixed_threshold();
//    int exceedance = 0;
//    for(i = 0;i<Ncpy;i++)
//    {
//        if (cpy[i].re>threshold)
//            exceedance = TRUE;
//    }
//    if (exceedance)
//    {
//        for(i = 0;i<Ncpy;i++)
//            insert_into_storage_test(cpy[i].re);
//    }
    
    // Remove bias (not needed because of down conversion where DC is filtered)
//    int i;
    int32_t mean= (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
////    print_val("mean", mean);
    for (i=0;i<Ncpy;i++){
        cpy[i].re=cpy[i].re-mean;
    }
//    print_array_int32c(cpy, "CPY", Ncpy);
    
    
    // Downconvert all values
    mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
//    print_array_int32c(dc_vals, "DC_VALS", Ncpy);
    
    // Rescale previous operation (needed in order to avoid overflow)
    rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3

//    print_array_int32c(dc_vals, "DC_VALS_RESCALE", Ncpy);

//    Low pass filter (on pre&post padded buffer slice)
    conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);

//    print_val("Ntaps_dcfilt is ", Ntaps_dcfilt);

//    print_array_int32c(lpfilt, "LPFILT", Ntaps_dcfilt);

//    print_array_int32c(dc_vals_lpfilt, "DC_VALS_LPFILT", Ndc_passband);
//     Resample 
    downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);

//    print_array_int32c(dc_vals_lpfilt, "DC", Ndc_baseband);
    // Note: If programmer has wrecked correct functionality with 
    // dc_factor: Ndc_baseband should equal Ndc and 
    // Ndc_passband should equal floor(Fs/B)*Ndc, where Ndc equals
    // last value of loop variable inside downsample_overwrite_int32c().

    // Rescale filtered output (needed in order to avoid overflow)
    rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
//    print_array_int32c(dc_vals_lpfilt, "DC_RESCALE", Ndc_baseband);

//    print_dc_vals_lpfilt();
    // Save downconverted result 
    /*for (i=0;i<Ncpy;i++){
        //printInt(cpy[i].re); printDelim(); printEL();
        printLongInt(dc_vals_lpfilt[i].re); printDelim(); printLongInt(dc_vals_lpfilt[i].im); printDelim(); printEL();
    }*/
}

// Function for actually triggering downconversion
void downconvert_corr_fixed_thresholding(int Nvals)
{
    Ndc_baseband=Nvals/dc_factor;  // Compute number of baseband values 
    Ndc_passband=Ndc_baseband*dc_factor; // Compute number of passband values 

    int32c *ctime = get_ctime();
    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();
    
    int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);
    int Ncpy=Nbpad_dcfilt+Ndc_passband+Nfwpad_dcfilt;
    int i;
    int threshold = get_fixed_threshold();
    int threshold_dc = get_fixed_threshold_dc();
    int exceedance = 0;
    int exceedance_dc = 0;
    
    for(i = 0;i<Ncpy;i++)
    {
        if (cpy[i].re>threshold)
            exceedance = TRUE;
    }
    if (exceedance)
    {
        for(i = 0;i<Ncpy;i++)
            insert_into_storage(cpy[i].re);
        
        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);

        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3

    //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);

    //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);

    // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
        
    // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        
        for(i = 0;i<NFFT_DC;i++)
            if (abs_32c(ctime[i])>threshold_dc)
            {
                exceedance_dc = TRUE;
                break;
            }

        if (exceedance_dc)
            for(i = 0;i<NFFT_DC;i++)
                insert_into_storage_test(abs_32c(ctime[i]));
    }
}

// Function for actually triggering downconversion
int downconvert_corr_adapted_thresholding(int Nvals)
{
    Ndc_baseband = Nvals/dc_factor;  // Compute number of baseband values 
    Ndc_passband = Ndc_baseband*dc_factor; // Compute number of passband values 

    int32c *ctime = get_ctime();
    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();

    int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

    int Ncpy=Nbpad_dcfilt+Ndc_passband+Nfwpad_dcfilt;
    int i;

    int32_t mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
    for (i=0;i<Ncpy;i++)
    {
        cpy[i].re=cpy[i].re-mean;
//            insert_into_storage_test(cpy[j].re);
    }

//    int exceedance = 0;
    int32_t abs_corr_time[NFFT_DC] = {0};
//    int32_t abs_dc_vals[Ndc_baseband];
    int32_t threshold_dc = 0;

//    int threshold = get_fixed_threshold();
    // Downconvert all values
    mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);

    // Rescale previous operation (needed in order to avoid overflow)
    rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3

//    Low pass filter (on pre&post padded buffer slice)
    conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);

//     Resample 
    downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);

// Rescale filtered output (needed in order to avoid overflow)
    rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

//    calculate_abs_corr_time(abs_dc_vals, dc_vals_lpfilt, Ndc_baseband);
//
//    for(i = 0;i<Ndc_baseband;i++)
//    {
//        insert_into_storage(abs_dc_vals[i]);
////        insert_into_storage(dc_vals_lpfilt[i].im);
//    }

// correlation in time domain
    conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
    rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);
    calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);
    
    for(i = 0;i<NFFT_DC;i++)
        insert_into_storage(abs_corr_time[i]);
//    rescale_complex2max_int32c(ctime,NFFT_DC,2048*RESCALE_FXP);

    threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
    insert_into_storage_test(threshold_dc);
//    int exceedance = FALSE;
    
    for(i = 0;i<NFFT_DC;i++)
        if (abs_corr_time[i]>threshold_dc)
        {
            exceedance = TRUE;
//            return TRUE;
//            set_index_dc_exceedance(i);
            break;
        }
    if (exceedance)
    {
        for(i = 0;i<NFFT_DC;i++)
            insert_into_storage_test(abs_corr_time[i]);
        return TRUE;
    }
    return FALSE;
}

int max_index_array_int32_t(int32_t *p, int N)
{
    int index = 0;
    int32_t temp = 0;
    int i;
    for (i = 0;i<N;i++)
        if (temp<p[i])
        {
            temp = p[i];
            index = i;
        }
    return index;
}

int first_exceedance_index_array_int32_t(int32_t *p, int N, int32_t threshold)
{
    int index = -1;
    int i;
    for (i = 0;i<N;i++)
        if (p[i]>threshold)
        {
            index = i;
            break;
        }
    return index;
}

int max_index_array_int16_t(int16_t *p, int N)
{
    int index = 0;
    int16_t temp = 0;
    int i;
    for (i = 0;i<N;i++)
        if (temp<p[i])
        {
            temp = p[i];
            index = i;
        }
    return index;
}

void test_interaction(void)
{
    int Nframes = 500;
    int j = 0;
    start_sampling(DEFAULT_SAMPLING_FREQUENCY,.5);
    while(j++<Nframes)
    {
        acquire_samples(get_Nframe());
//        printString("acquire samples is fine!"); printEL();
//        print_val_int(get_adc_buffer_valcnt());
        int16_t* p=unload_adc_buffer(Nframe,0,0);
        
        if ((j%10)==0)
        {
            stop_sampling();
//            send_pulse(60000, SINE, 0);
            print_val("trigger index", j);
            start_sampling(DEFAULT_SAMPLING_FREQUENCY,.5);
        }
//        printString("pulse is sent!"); printEL();
//        print_val_int(get_adc_buffer_valcnt());
//        printString("done sampling!");
//        delaymilli(1);
    }
    printString("true end!"); printEL();
}

void test_env_sampling(void)
{
    int Nframes = 312;
    int Nframe = get_Nframe();
    int Ndc_passband = get_Ndc_passband();
    int Ndc_baseband = get_Ndc_baseband();
    int Ncpy = get_Ncpy();
    int Nref_baseband = get_Nref_baseband();
    int32_t mean;
    int32_t abs_corr_time[NFFT_DC];
    int32_t threshold_dc;
    int32c * ctime = get_ctime();
    int32c * refsig_time = get_refsig_time();
    int index_max;

    print_val("Nframe", Nframe);
    print_val("Ndc_passband", Ndc_passband);
    print_val("Ndc_baseband", Ndc_baseband);
    print_val("Ncpy", Ncpy);
    print_val("Nframes", Nframes);
    print_val("Nref_baseband", Nref_baseband);
    print_val("frequency", get_frequency());

    int i;
    start_sampling(get_Fs(),.5);
    print_start_mark("VALCNT");
    while(Nframes--)
    {
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);
        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
            cpy[i].re=cpy[i].re-mean;
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);
        for(i = 0;i<NFFT_DC;i++)
            insert_into_storage(abs_corr_time[i]);

        threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
        insert_into_storage_test(threshold_dc);

        index_max = max_index_array_int32_t(abs_corr_time, NFFT_DC);
        insert_into_storage_test(index_max);
        insert_into_storage_test(abs_corr_time[index_max]);

        print_val_int(get_adc_buffer_valcnt());
    }
    print_end_mark("VALCNT");
    stop_sampling();
}

void beacon(void)
{
    // Time trackers
    Time t_arrival;     // time of arrival in the buffer
    Time t_current; // time at current operation
    Time t_delay;       // additional delay to fixed the reference
    Time t_passed;      // the operation time

    int Nframe = get_Nframe();   
    int Ndc_baseband = get_Ndc_baseband();
    int Ndc_passband = get_Ndc_passband();

    int32c *ctime = get_ctime();
    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();

    int32_t abs_corr_time[NFFT_DC] = {0};
    int32_t threshold_dc = 0;
    int32_t mean;   // bias removal
    timeout_cnt = 0;

//    int index_max;
    int index_exceedance = -1;  // exceedance index
    int Ncpy = get_Ncpy();
    int i;
    int Nth_Frame = 0;
    int Fs = get_Fs();
    long int tcmp0 = 0;

    print_val("Nframe", Nframe);
    print_val("Ndc_baseband", Ndc_baseband);
    print_val("Ndc_passband", Ndc_passband);
    print_val("Nref_baseband", Nref_baseband);
    print_val("Ncpy", Ncpy);
    print_val("Nfwpad_dcfilt", Nfwpad_dcfilt);
    print_val("Nbpad_dcfilt", Nbpad_dcfilt);
    print_val("Fs", Fs);
    print_val("dc_factor", get_dc_factor());

    printString("beacon starts working! Good luck! "); printEL();
//    print_start_mark("VALCNT");

    print_start_mark("TRIGGER_INDEX");
    start_sampling(Fs,.5);
    while(TRUE){
//        int flag_save = FALSE; // SAVE THE ADC OR NOT, FLAG
//        t_latest_sample.clk=get_time_clk();
        TMR4 = 0;  // initialise the timer so no overflow will happen
//        index_exceedance = -1;

// MF part ---------------------------------------------------------------------
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
//        printString("acquire samples is fine!"); printEL();
//        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        // remove the bias, or the dc component of the signal
        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
        {
//            if(cpy[i].re>fixed_threshold)
//                flag_save = TRUE;
            cpy[i].re=cpy[i].re-mean;
        }

        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

        // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

        for(i = 0;i<NFFT_DC;i++)
            insert_into_storage(abs_corr_time[i]);
//            insert_raw_data_into_buffer(abs_corr_time[i]);

//        if(flag_save)
//        {
////            printString("flag is true!"); printEL();
//            for(i = 0;i<Ncpy;i++)
//            {
//                insert_raw_data_into_buffer(cpy[i].re);
//            }
//        }

        threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
        insert_into_storage_test(threshold_dc);
//        index_max = max_index_array_int32_t(abs_corr_time, NFFT_DC);
        index_exceedance = first_exceedance_index_array_int32_t(abs_corr_time, NFFT_DC, threshold_dc);
        insert_into_storage_test(index_exceedance);
//        insert_into_storage_test(abs_corr_time[index_max]);

    // EOF OF MF part ----------------------------------------------------------
//        print_val_int(get_adc_buffer_valcnt());
        if (timeout_cnt++>=TIMEOUT_FRAMES_BEACON)
        {
            stop_sampling();
//            print_end_mark("VALCNT");
            print_end_mark("TRIGGER_INDEX");

//            print_val_int(timeout_cnt);
//            printString("time is out!"); printEL();
            transmit_storage_vector();
            transmit_storage_vector_test();
            print_threshold_buffer();
            print_raw_data_buffer();
            print_adc_buffer();
            return;
        }
//        if(abs_corr_time[index_max]>threshold_dc)
        if(index_exceedance != -1){       
            stop_sampling();
//            print_end_mark("VALCNT");

//            set_startdelay(1000000);
            t_buffer = get_t_buffer();  // comes from the timer2handler
            t_arrival.clk = t_buffer.clk + sampl2clk(index_exceedance*dc_factor, Fs); // t_arrival is the time when signal arrives in the buffer
            
            tcmp0=t_arrival.clk;
            
            t_current.clk = get_time_clk();
            t_passed.clk = t_current.clk - t_arrival.clk;
            t_delay.clk = sec2clk(fixed_delay) - t_passed.clk;

//            print_val_int(get_adc_buffer_valcnt());
//
////            Time t1, t2;
////            t1.clk = get_time_clk();

            delay_clk(t_delay.clk);
//            tcmp0=TMR4;
////            delaymicrosec((int)(MICRO_FACTOR*clk2sec(t_delay.clk)));
////            delaymicro_calibrated((int)(MICRO_FACTOR*clk2sec(t_delay.clk)));
////            delaymicro((int)(MICRO_FACTOR*clk2sec(t_delay.clk)));
////            t2.clk = get_time_clk();

            send_pulse(get_frequency(), get_bandwidth(), SINE);

//            send_square_pulse(10, 40000, 100);
            delaymilli(100);
            printString("t_delay is ");  print_val_double(
                    clk2sec(get_t_pulsestart()-tcmp0)); printEL();

            printString("startdelay is  "); printLongInt(get_startdelay()); printEL();
            printString("Nth_Frame is "); print_val_int(Nth_Frame);

            print_val("fixed_delay", fixed_delay);
            print_val("t_buffer", t_buffer.clk);
            print_val("t_arrival", t_arrival.clk);
            print_val("t_pulsestart", get_t_pulsestart());
            print_val("t_current", t_current.clk);
            print_val("t_passed", t_passed.clk);
            print_val("t_delay", t_delay.clk);
            print_val("sampl2clk", sampl2clk(index_exceedance*dc_factor, Fs));
            print_val("index_exceedance", index_exceedance);

            print_val_int(timeout_cnt); 
//            print_array_int32_t(&abs_corr_time, "CORR_TIME", NFFT_DC);
//            print_array_int32_t(&threshold_dc, "THRESHOLD_DC", 1);;
//            print_array_int32_t(&index_exceedance, "INDEX", 1);
//            transmit_storage_vector();
//            transmit_storage_vector_test();
//            print_threshold_buffer();
//            print_raw_data_buffer();
//            print_adc_buffer();
//                print_threshold_buffer();

            for(i = 0;i<Ncpy;i++)
                insert_raw_data_into_buffer(cpy[i].re);
            
            index_exceedance = -1;
            start_sampling(Fs, .5);
//            delaymilli(1000); // delay to flush the entire buffer from previous values
        }
        Nth_Frame++;
    }

//    print_end_mark("THRESHOLD_DC");
//    print_end_mark("VALCNT");
//    printString("true end!"); printEL();
}

void beacon_2nd(void)
{
    // Time trackers
    Time t_arrival;     // time of arrival in the buffer
    Time t_begin;     // time to begin the operation
    Time t_end;     // time to begin the operation
    Time t_current; // time at current operation
//    Time t_buffer;  // buffer unloading time
    Time t_delay;       // additional delay to fixed the reference
    Time t_passed;      // the operation time

    int Nframe = get_Nframe();   
    int Ndc_baseband = get_Ndc_baseband();
    int Ndc_passband = get_Ndc_passband();

    int32c *ctime = get_ctime();
    int Nref_baseband = get_Nref_baseband();
    int32c *refsig_time = get_refsig_time();

    int32_t abs_corr_time[NFFT_DC] = {0};
    int32_t threshold_dc = 0;
    int32_t mean;   // bias removal
//    int numOfFlags = 0;
    timeout_cnt = 0;

    int index_max;
    int Ncpy = get_Ncpy();
    int i;
    int fixed_threshold = get_fixed_threshold();
    int Fs = get_Fs();
    print_val("Nframe", Nframe);
    print_val("Ndc_baseband", Ndc_baseband);
    print_val("Ndc_passband", Ndc_passband);
    print_val("Nref_baseband", Nref_baseband);
    print_val("Ncpy", Ncpy);
    print_val("Nfwpad_dcfilt", Nfwpad_dcfilt);
    print_val("Fs", Fs);

    printString("beacon starts working! Good luck! "); printEL();

    print_start_mark("VALCNT");

//    print_start_mark("TRIGGER_INDEX");
    TMR4 = 0;  // initialise the timer so no overflow will happen
    start_sampling(Fs,.5);
    while(TRUE)
    {
        int flag_save = FALSE; // SAVE THE ADC OR NOT, FLAG
//        t_latest_sample.clk=get_time_clk();
// MF part ---------------------------------------------------------------------
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
//        printString("acquire samples is fine!"); printEL();
        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        // remove the bias, or the dc component of the signal
        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
        {
            if(cpy[i].re>fixed_threshold)
                flag_save = TRUE;
            cpy[i].re=cpy[i].re-mean;
        }

        if(flag_save)
        {
            for(i = 0;i<Ncpy;i++)
            {
                insert_raw_data_into_buffer(cpy[i].re);
            }
        }
        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

        // correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_BEACON*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

        for(i = 0;i<NFFT_DC;i++)
            insert_into_storage(abs_corr_time[i]);

        threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
        insert_into_storage_test(threshold_dc);
        index_max = max_index_array_int32_t(abs_corr_time, NFFT_DC);
        insert_into_storage_test(index_max);
        insert_into_storage_test(abs_corr_time[index_max]);

    // EOF OF MF part ----------------------------------------------------------
        print_val_int(get_adc_buffer_valcnt());

        if (timeout_cnt++>=TIMEOUT_FRAMES_BEACON)
        {
            stop_sampling();
            print_end_mark("VALCNT");
//            print_val_int(timeout_cnt);
            printString("time is out!"); printEL();
            transmit_storage_vector();
            transmit_storage_vector_test();
            print_threshold_buffer();
            print_raw_data_buffer();
            return;
        }
        if(abs_corr_time[index_max]>threshold_dc)
        {
            stop_sampling();
            
            t_buffer = get_t_buffer();  // comes from the timer2handler
            t_arrival.clk = t_buffer.clk + sampl2clk(index_max*dc_factor, Fs); // t_arrival is the time when signal arrives in the buffer
            t_begin.clk = t_buffer.clk;
            t_end.clk = get_time_clk();
            t_current.clk = t_buffer.clk + t_end.clk - t_begin.clk;

            t_passed.clk = t_current.clk - t_arrival.clk;
            t_delay.clk = fixed_delay - t_passed.clk;

//            print_val("t_buffer", t_buffer.clk);
//            print_val("t_arrival", t_arrival.clk);
//            print_val("t_begin", t_begin.clk);
//            print_val("t_end", t_end.clk);
//            print_val("t_current", t_current.clk);
//            print_val("t_passed", t_passed.clk);
//            print_val("t_delay", t_delay.clk);
//            print_val("fixed_delay", fixed_delay);
//            print_val("sampl2clk", sampl2clk(index_max*dc_factor, Fs));
//            print_val("index_max", index_max);

//            print_val("t_delay", (int)(MICRO_FACTOR*clk2sec(t_delay.clk)));

//            print_val_double(clk2sec(t_delay.clk));
//            print_val_int(get_adc_buffer_valcnt());

//            t_begin.clk = get_time_clk();
            delay_clk(t_delay.clk);
//            delaymicro_calibrated((int)(MICRO_FACTOR*clk2sec(t_delay.clk)));
//            delaymicro((int)(MICRO_FACTOR*clk2sec(t_delay.clk)));

//            t_end.clk = get_time_clk();
//
//            printString("delayed time is ");
//            print_val_double(clk2sec(t_end.clk - t_begin.clk));

            send_pulse(60000, SINE, TRUE);

//            print_val_int(timeout_cnt);
//            printString("pulse is sent!"); printEL();
            delaymilli(100);
//            if (timeout_cnt>100)
//            {
//                transmit_storage_vector();
//                transmit_storage_vector_test();
//                print_threshold_buffer();
//                break;
//            }
//            adc_buffer_valcnt = 0;
            TMR4 = 0;  // initialise the timer so no overflow will happen
            start_sampling(Fs, .5);
        }
    }
//    print_end_mark("THRESHOLD_DC");
//    print_end_mark("VALCNT");
    printString("true end!"); printEL();
}

void transmitter(void)
{
//    printString("Transmitter is ready! Range is being estimated! Please wait!"); printEL();
    // Time trackers
    timeout_cnt = 0;
    Time t_pulse;
    Time TOF;
    Time t_arrival;     // time of arrival in the buffer
    set_storage_windex(0); // each time to
    set_storage_windex_test(0); // each time to
    
    double range; // range of estimation
    int index_exceedance = -1; // max index of the exceedance

    int Nframe = get_Nframe();    
    int Nref_baseband = get_Nref_baseband();
    Ndc_baseband = get_Ndc_baseband();
    Ndc_passband = get_Ndc_passband();
    int32c *ctime = get_ctime();
    int32c *refsig_time = get_refsig_time();
    int Ncpy = get_Ncpy();
    int Fs = get_Fs();
    int32_t abs_corr_time[NFFT_DC] = {0};
    int32_t threshold_dc = 0;
    int32_t mean;
    int i, Nth_Frame;
    Nth_Frame = 0;
    print_val("Nframe", Nframe);
    print_val("Nref_baseband", Nref_baseband);
    print_val("Nfwpad_dcfilt", Nfwpad_dcfilt);
    print_val("Nbpad_dcfilt", Nbpad_dcfilt);
    print_val("Ndc_baseband", Ndc_baseband);
    print_val("Ndc_passband", Ndc_passband);
    print_val("Ncpy", Ncpy);
    print_val("frequency", get_frequency());
    print_val("Nth_Frame", Nth_Frame);

    TMR4 = 0;
    Time t1, t2;
    t1.clk = get_time_clk();
//    send_pulse(40000, SINE, 0);  // already integrated inside the pulse, an additional delay is inserted
//    send_square_pulse(10, 40000, 100);
    send_pulse(get_frequency(), get_bandwidth(), SINE);
//    TOF_begin.clk = t_pulse.clk;      // assume the function call does not take extra time clock
//    TOF.clk = get_time_clk() - t_pulse.clk;
    long int T0,T1;

//    delaymilli(900);

    
    start_sampling(Fs,.5);
//    delaymilli(500);
    t2.clk = get_time_clk();
//    print_start_mark("VALCNT");
    T0 = get_time_clk();
    while(TRUE)
    {
    // MF part ---------------------------------------------------------------------
//        t0=get_time_clk();
        //t0=clk2sec(TMR4);
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
        //t1=clk2sec(TMR4);
        //printString("t=");printDouble(t1-t0);printEL();
        
        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        // remove the bias, or the dc component of the signal
        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
        {
            cpy[i].re=cpy[i].re-mean;
//            insert_raw_data_into_buffer(cpy[i].re);
        }
        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

//         correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_TRANSMITTER*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

        for(i = 0;i<NFFT_DC;i++)
        {
            insert_into_storage(abs_corr_time[i]); 
//            insert_into_storage(ctime[i].re); 
//            insert_into_storage(ctime[i].im); 
        }

        threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
        insert_into_storage_test(threshold_dc);
        index_exceedance = first_exceedance_index_array_int32_t(abs_corr_time, NFFT_DC, threshold_dc);
        insert_into_storage_test(index_exceedance);

//        if (index_exceedance!= -1)
//            for(i = 0;i<Ncpy;i++)
//                insert_raw_data_into_buffer(cpy[i].re);

//        printString("index_exceedance is "); printInt(index_exceedance); printEL();
        print_val_int(get_adc_buffer_valcnt());
//        t1=get_time_clk();
    // EOF OF MF part ----------------------------------------------------------
        if(timeout_cnt++>=TIMEOUT_FRAMES_TRANSMITTER)
        {
            stop_sampling();
            print_end_mark("VALCNT");
//            printDouble(clk2sec(t1)-clk2sec(t0));printEL();
//            print_val("timeout", timeout_cnt);
//            printString("Time is out! Beacon did not respond :-< so sad! "); printEL();
//            print_val("TOF ", TOF.clk);

//            for(i = 0;i<Ncpy;i++)
//                insert_raw_data_into_buffer(cpy[i].re);
            printString("the time between send pulse and start_listen is "); printInt(t2.clk - t1.clk); printEL();
            printString("delayed time is "); printInt(T1 - T0); printEL();

//            print_array_int32_t(abs_corr_time, "CORR_TIME", NFFT_DC);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            print_array_int32_t(&index_exceedance, "INDEX", 1);
//            printDouble(range); printDelim(); printEL();
            print_val_int(t_arrival.clk);
            print_val_int(TOF.clk);
//            print_val("TOF", TOF.clk);
            print_val_int(fixed_delay);
            print_val_int(index_exceedance);
            print_val("Nth_Frame", Nth_Frame);

            transmit_storage_vector();
            transmit_storage_vector_test();
            print_threshold_buffer();
            print_raw_data_buffer();
            print_adc_buffer();
            
            return;
        }
//        if(abs_corr_time[index_max]>threshold_dc)
        if(index_exceedance != -1)
        {
            print_end_mark("VALCNT");
            stop_sampling();
            t_buffer = get_t_buffer();
            t_arrival.clk = t_buffer.clk + sampl2clk(index_exceedance*dc_factor, Fs); // t_arrival is the time when signal arrives in the buffer
            t_pulse.clk = get_t_pulsestart();
            TOF.clk = t_arrival.clk - fixed_delay*FCY - t_pulse.clk;// - sec2clk(Nth_Frame / MICRO_FACTOR); 
//             - COMPENSATED_TIME
            T1 = t_arrival.clk;
            printString("T1 - T0 is "); printDouble(clk2sec(T1 - T0)); printEL();
   
            printString("t_arrival is "); print_val_double(clk2sec(t_arrival.clk));
            printString("TOF is "); print_val_double(clk2sec(TOF.clk));
            printString("fixed_delay is "); print_val_double(clk2sec(fixed_delay*FCY));
            print_val("index_exceedance", index_exceedance);
            print_val("sound speed", get_soundspeed());
            print_val("Nth_Frame", Nth_Frame);

            range = clk2sec(TOF.clk)/2*get_soundspeed();
            printDouble(range); printDelim(); printEL();

            printDouble(get_time_clk() - t_pulse.clk); printEL();

            printString("The distance between two transducers is "); printDouble(range); printString(" meters! "); printEL();
            for(i = 0;i<Ncpy;i++)
                insert_raw_data_into_buffer(cpy[i].re);

//            print_array_int32_t(abs_corr_time, "CORR_TIME", NFFT_DC);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            print_array_int32_t(&index_exceedance, "INDEX", 1);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            transmit_storage_vector();
//            transmit_storage_vector_test();
//            print_raw_data_buffer();
//            return;
            index_exceedance = -1;
            start_sampling(Fs, 0.5);
        }
        Nth_Frame++;
//        TOF_end.clk = get_time_clk();
//        TOF.clk += TOF_end.clk - TOF_begin.clk;
//        TOF_begin.clk = get_time_clk();
//        print_val("TOF_end", TOF_end.clk);
//        print_val("TOF_begin", TOF_begin.clk);
//        print_val("TOF", TOF.clk);
    }
}

void transmitter_2nd(void)
{
//    printString("Transmitter is ready! Range is being estimated! Please wait!"); printEL();

    // Time trackers
    timeout_cnt = 0;
    Time t_pulse;
    Time TOF;
    Time t_arrival;     // time of arrival in the buffer

//    Time TOF_begin;
//    Time TOF_end;
//    Time t_begin;     // time to begin the operation
//    Time t_end;     // time to begin the operation
//    int32_t t_passed = 0;
    
    double range; // range of estimation
//    int index_max; // max index of the exceedance
    int index_exceedance = -1; // max index of the exceedance

    int Nframe = get_Nframe();    
    int Nref_baseband = get_Nref_baseband();
    Ndc_baseband = get_Ndc_baseband();
    Ndc_passband = get_Ndc_passband();
    int32c *ctime = get_ctime();
    int32c *refsig_time = get_refsig_time();
    int Ncpy = get_Ncpy();
    int Fs = get_Fs();
    int32_t abs_corr_time[NFFT_DC] = {0};
    int32_t threshold_dc = 0;
    int32_t mean;
    
    int i, Nth_Frame;
    Nth_Frame = 0;
//    print_val("Nframe", Nframe);
//    print_val("Nref_baseband", Nref_baseband);
////    print_val("Nref_passband", Nref_passband);
//    print_val("Nfwpad_dcfilt", Nfwpad_dcfilt);
//    print_val("Nbpad_dcfilt", Nbpad_dcfilt);
//    print_val("Ndc_baseband", Ndc_baseband);
//    print_val("Ndc_passband", Ndc_passband);
//    print_val("Ncpy", Ncpy);
//    print_val("frequency", get_frequency());
    TMR4 = 0;
    Time t1, t2;
    t1.clk = get_time_clk();
//    send_pulse(40000, SINE);  // already integrated inside the pulse, an additional delay is inserted
//    TOF_begin.clk = t_pulse.clk;      // assume the function call does not take extra time clock
//    TOF.clk = get_time_clk() - t_pulse.clk;
//    print_start_mark("VALCNT");
//    long int t0,t1;
    t2.clk = get_time_clk();
    start_sampling(Fs,.5);
    while(TRUE)
    {
    // MF part ---------------------------------------------------------------------
//        t0=get_time_clk();
        //t0=clk2sec(TMR4);
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
        //t1=clk2sec(TMR4);
        //printString("t=");printDouble(t1-t0);printEL();
        
//        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        // remove the bias, or the dc component of the signal
        mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
        {
            cpy[i].re=cpy[i].re-mean;
//            insert_raw_data_into_buffer(cpy[i].re);
        }
        // Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
        // Rescale filtered output (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);

//         correlation in time domain
        conv_prepadded_32c(ctime,NFFT_DC,dc_vals_lpfilt,refsig_time,Nref_baseband);
        rescale_complex2max_int32c(ctime,NFFT_DC,LIFT_FACTOR_TRANSMITTER*RESCALE_FXP);
        calculate_abs_corr_time(abs_corr_time, ctime, NFFT_DC);

        for(i = 0;i<NFFT_DC;i++)
        {
            insert_into_storage(abs_corr_time[i]); 
//            insert_into_storage(ctime[i].re); 
//            insert_into_storage(ctime[i].im); 
        }

        threshold_dc = get_threshold_dc_avg(abs_corr_time, NFFT_DC);
        insert_into_storage_test(threshold_dc);
//        index_max = max_index_array_int32_t(abs_corr_time, NFFT_DC);
        index_exceedance = first_exceedance_index_array_int32_t(abs_corr_time, NFFT_DC, threshold_dc);
        insert_into_storage_test(index_exceedance);

//        if (index_exceedance!= -1)
//            for(i = 0;i<Ncpy;i++)
//                insert_raw_data_into_buffer(cpy[i].re);
        
//        printString("index_exceedance is "); printInt(index_exceedance); printEL();
//        print_val_int(get_adc_buffer_valcnt());
//        t1=get_time_clk();
    // EOF OF MF part ----------------------------------------------------------
        if(timeout_cnt++>=TIMEOUT_FRAMES_TRANSMITTER)
        {
            stop_sampling();
//            print_end_mark("VALCNT");
//            printDouble(clk2sec(t1)-clk2sec(t0));printEL();
//            print_val("timeout", timeout_cnt);
//            printString("Time is out! Beacon did not respond :-< so sad! "); printEL();
//            print_val("TOF ", TOF.clk);
            
//            for(i = 0;i<Ncpy;i++)
//                insert_raw_data_into_buffer(cpy[i].re);
            printString("the time between send pulse and start_listen is "); printInt(t2.clk - t1.clk); printEL();

//            print_array_int32_t(abs_corr_time, "CORR_TIME", NFFT_DC);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            print_array_int32_t(&index_exceedance, "INDEX", 1);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
            printDouble(range); printDelim(); printEL();
            print_val_int(t_arrival.clk);
            print_val_int(TOF.clk);
//            print_val("TOF", TOF.clk);
            print_val_int(fixed_delay);
            print_val_int(index_exceedance);
            print_val("Nth_Frame", Nth_Frame);
            
            transmit_storage_vector();
            transmit_storage_vector_test();
            print_raw_data_buffer();
            
            return;
        }
//        if(abs_corr_time[index_max]>threshold_dc)
        if(index_exceedance != -1)
        {
//            stop_sampling();
//            print_end_mark("VALCNT");
            t_buffer = get_t_buffer();
//            t_passed = t_buffer.clk - TOF_begin.clk; 
//            t_arrival.clk = t_buffer.clk + sampl2clk(index_max*dc_factor, Fs); // t_arrival is the time when signal arrives in the buffer
            t_arrival.clk = t_buffer.clk + sampl2clk(index_exceedance*dc_factor, Fs); // t_arrival is the time when signal arrives in the buffer
//            printString("t_arrival is"); print_val_double(clk2sec(t_arrival.clk));
//            print_val("t_arrival ", t_arrival.clk);

            t_pulse.clk = get_t_pulsestart();

            TOF.clk = t_arrival.clk - fixed_delay*MICRO_FACTOR - t_pulse.clk;// - sec2clk(Nth_Frame / MICRO_FACTOR); 
//            printString("TOF after subtraction is"); print_val_double(clk2sec(TOF.clk));
//            printString("fixed_delay is"); print_val_double(clk2sec(fixed_delay));
//
//            print_val("fixed_delay", fixed_delay);
//            print_val("index_exceedance", index_exceedance);
//            print_val("sound speed", get_soundspeed());

            range = clk2sec(TOF.clk)/2*get_soundspeed();

            printDouble(range); printDelim(); printEL();
            print_val_int(t_arrival.clk);
            print_val_int(TOF.clk);
////            print_val("TOF", TOF.clk);
//            print_val_int(fixed_delay);
//            print_val_int(index_exceedance);
            print_val("Nth_Frame", Nth_Frame);

//            printString("The distance between two transducers is "); printDouble(range); printString(" meters! "); printEL();
            
            for(i = 0;i<Ncpy;i++)
                insert_raw_data_into_buffer(cpy[i].re);

//            print_array_int32_t(abs_corr_time, "CORR_TIME", NFFT_DC);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            print_array_int32_t(&index_exceedance, "INDEX", 1);
//            print_array_int32_t(&threshold_dc, "THRESHOLD", 1);
//            transmit_storage_vector();
//            transmit_storage_vector_test();
//            print_raw_data_buffer();
//            return;
        }
        Nth_Frame++;
//        TOF_end.clk = get_time_clk();
//        TOF.clk += TOF_end.clk - TOF_begin.clk;
//        TOF_begin.clk = get_time_clk();
//        print_val("TOF_end", TOF_end.clk);
//        print_val("TOF_begin", TOF_begin.clk);
//        print_val("TOF", TOF.clk);
    }
}

void test_valcnt_calibration(void)
{
    int Nframe = get_Nframe();    
    int Nref_baseband = get_Nref_baseband();
    Ndc_baseband = get_Ndc_baseband();
    Ndc_passband = get_Ndc_passband();
    int32c *ctime = get_ctime();
    int32c *refsig_time = get_refsig_time();
    int Ncpy = get_Ncpy();
    int i, j;
    
   int Nframes = 100;
    start_sampling(get_Fs(), 0.5);
    print_start_mark("VALCNT");
    while(Nframes--)
    {
        acquire_samples(Ndc_passband + Nfwpad_dcfilt);
//        print_val_int(get_adc_buffer_valcnt());
        print_val_int(get_adc_buffer_valcnt());

        int16c* cpy=unload_adc_buffer_complex(Ndc_passband,Nbpad_dcfilt,Nfwpad_dcfilt);

        // remove the bias, or the dc component of the signal
        int32_t mean = (int32_t) (sum_16c_real(cpy,Ncpy)/Ncpy); // Sum over real part (adc values)
        for (i=0;i<Ncpy;i++)
        {
            cpy[i].re=cpy[i].re-mean;
//            insert_raw_data_into_buffer(cpy[i].re);
        }

//         Downconvert all values
        mult_complex_arr(dc_vals,cpy,dc_exponential,Ncpy);
        // Rescale previous operation (needed in order to avoid overflow)
        rescale_complex2max_int32c(dc_vals,Ncpy,RESCALE_FXP); // 5us/sampl @ 160e3
        //    Low pass filter (on pre&post padded buffer slice)
        conv_prepadded_32c(dc_vals_lpfilt,Ndc_passband,dc_vals,lpfilt,Ntaps_dcfilt);
        //     Resample 
        downsample_overwrite_int32c(dc_vals_lpfilt,Fs,dc_Fs,Ndc_passband);
//        // Rescale filtered output (needed in order to avoid overflow)
//        rescale_complex2max_int32c(dc_vals_lpfilt,Ndc_baseband,RESCALE_FXP);
        
        print_val_int(get_adc_buffer_valcnt());
    }
    print_end_mark("VALCNT");
}

void test_ringing_effect_bleeding(void)
{
    int i;
    int start_delay = 0;
    Time t1, t2;
    for (i = 0;i<20;i++)
    {
        start_delay += i * 500;
        set_startdelay(start_delay);
//        send_pulse(40000, SINE, 0);
        t1.clk = get_t_pulsestart();
        t2.clk = get_time_clk();
        listen_full_storage();
        print_val("SD", get_startdelay());
        printString("Time difference is ");
        print_val_double(clk2sec(t2.clk - t1.clk));
    }
    printString("done!"); printEL();
}


// Sampling functions ----------------------------
void start_sampling(long int Fs_, double sampl_timeout_){
    index_adcbuffer_oldest = index_adcbuffer_nextsmpl;
   // Reset counter for non-emptied values in buffer
    adc_buffer_valcnt=0;
    
    
    // Set timeout variable (timeout in [ms])
    timeout_sampl.clk=sec2clk(sampl_timeout_);
    //sampl_timeout=sec2sampl(sampl_timeout_,Fs_);
 
    // Configure sampling timer
    T2CONbits.TON = 0; // Disable sampling timer
    IFS0bits.T2IF = 0; // Clear the timer interrupt status flag
    IEC0bits.T2IE = 1; // Enable interrupt for Timer2
    IPC2bits.T2IP = 1; // Set priority 1
    T2CONbits.TCKPS = 0b000;
//    printString("partA tricks okay"); printEL();
    // Set sampling rate
    set_sampling_rate(Fs_);
    // Set hardware to input 
    
    setTransducerToSensing();
//    printString("set to sensing is fine!"); printEL();
    delaymicrosec(360);   // delay to remove the bias from the ADC, or remove the capacitor charging effect

    // Configure ADC 
    ADCCON3bits.VREFSEL = 1;
    //ADCCON1bits.ON = 1;
    ADCCON1bits.ON = 1; // needs a delay avoid lagging problem
    delaymicrosec(3);
//    while(ADCCON1bits.ON!=1);
    ADCCSS1bits.CSS15 = 0; //AN15 (Class 3) quit scanning
    ADCCSS1bits.CSS18 = 1; //AN18 (Class 3) set for scan
    ADCCON3bits.GSWTRG = 1;
    // Activate sampling timer 
//    printString("ADC is fine!"); printEL();
//    delaymicro_calibrated(1);
    delaymicrosec(1);
//    Nop();
//    Nop();
//    Nop();
    T2CONbits.TON = 1; 
    // Initiate first value to timer (for the timeout algo)
//    printString("partB tricks okay"); printEL();
    t_latest_sample.clk=get_time_clk();
    
//    flush the buffer?
    acquire_samples(Nbpad_dcfilt);
    int16_t * temp = unload_adc_buffer(Nbpad_dcfilt, 0, 0);
//        int16c * temp = unload_adc_buffer_complex(Nbpad_dcfilt, 0, 0);
}

void stop_sampling(void){
    IEC0bits.T2IE = 0; // Disable interrupt for Timer2
    T2CONbits.ON = 0; //stop pulse out timer
}

void downsample_overwrite_int32c(int32c* seq, int r1, int r2, int Nr1)
{
    // Downsample Nr1 samples from r1 to r2 (r1>r2), results are 
    // stored by overwriting seq. Function returns Nr2, indicating 
    // how many downsampled values are stored in seq. Nr1 should be 
    // at least r1/r2 samples, and r1/r2 should be an exact integer. If
    // not, the output rate will be r1/floor(r1/r2) due to rounding.
    int Nnext=r1/r2; // Minimum number of samples in Nrl
    // Resample by returning every r1/r2 samples, starting with 
    // the FIRST sample
    int i=0;
    int inext = 0;
//    int inext=Nnext; // = i + Nnext
    while(inext<Nr1)
    {
        seq[i]=seq[inext];
        i++;
        inext += Nnext;
//        inext= i*Nnext;
    }
}

void acquire_samples(int N){
    if(N>adc_bufferlen){
        // Can't acquire more samples than one buffers worth
        N=adc_bufferlen;
    }
    // Wait until N samples have been acquired
    while(adc_buffer_valcnt<N){
//        print_val_int(get_adc_buffer_valcnt());
        // Wait for next sampled value
        if(timeout_sampl.clk<get_time_clk()-t_latest_sample.clk){
            // Timeout from sampling not working
            IEC0bits.T2IE = 0; // Disable interrupt for Timer2
            T2CONbits.ON = 0; //stop pulse out timer

            // Additionally print a specific warning to higher system
            printMessage(MSG_TIMEOUT_SAMPLING);
            return;        
        }
    }
}

// Version of acquire_samples() which obtains the correct number 
// of samples to use together with downconvert_adc_buffer(), which
// requires Nbpad_dcfilt values before, and Nfwpad_dcfilt values
// after the frame that is processed.
void acquire_samples_dcpadded(int N){
    acquire_samples(N+Nfwpad_dcfilt);
}
// Fill buffer with N samples and forget that they are there
void acquire_samples_forget(int N){
    acquire_samples(N);
    adc_buffer_valcnt=0;
}

void init_adc_buffer(void){
    // Fill the buffer with samples and reset the counter. 
    // USAGE MUST BE STAND-ALONE, IT STOPS THE SAMPLER AFTERWARDS
    // Needs to be put before another sampling
    start_sampling(DEFAULT_SAMPLING_FREQUENCY,.5);
    acquire_samples(get_adc_buffer_len());
//    acquire_samples(get_storage_vector_len());
    stop_sampling();
    adc_buffer_valcnt=0;
}

// (CADSON FUNCTION FOR CONTROLLING HARDWARE)
// Switch some transistors on the board, so that the transducer
// is connected to an input amplifier (turning it into a hydrophone)
void setTransducerToSensing(void) {
    Nop();
    Nop();
    Nop();
  
    // Make the transducer float
    LATDbits.LATD11 = 0; //1LoIn
    LATDbits.LATD9 = 0; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn
   /*
    // Make the transducer biased
    LATDbits.LATD0 = 0;
    LATDbits.LATD9 = 0;         
    LATDbits.LATD10 = 1;
    LATDbits.LATD11 = 1;   */

    // Enable input
    LATEbits.LATE1 = 1; //Enable 1
    LATEbits.LATE2 = 1; //Enable 2
    LATBbits.LATB13 = 0; //input enable
    LATBbits.LATB14 = 0; //input enable
    
    ANSELEbits.ANSE4 = 1;
    TRISEbits.TRISE4 = 1;
    Nop();
    Nop();
    Nop();
}

// Buffer functions ------------------------------
void insert_val_into_adcbuffer(int16_t val) {

    if (index_adcbuffer_nextsmpl >= adc_bufferlen) {
        index_adcbuffer_nextsmpl = 0;
    }
    adc_buffer[index_adcbuffer_nextsmpl] = val;
    index_adcbuffer_nextsmpl++;

    if(adc_buffer_valcnt<adc_bufferlen){
        adc_buffer_valcnt++; // Track number of inserted values
    }else{
        // Buffer is completely filled, put the "oldest" value one index ahead
        // of the newest sample
        index_adcbuffer_oldest=index_adcbuffer_nextsmpl;
    }
}

int16_t read_val_from_adcbuffer(void)
{
    int16_t val;
    if (index_adcbuffer_oldest >= adc_bufferlen) 
        index_adcbuffer_oldest = 0;
    
    val = adc_buffer[index_adcbuffer_oldest];
    index_adcbuffer_oldest++;
    if(adc_buffer_valcnt>0){
        adc_buffer_valcnt--;
    }else{
        // Flag if algorithm is extracting too many values for debugging
        buffer_overextract_flag=1;
    }
    return val;
}

int16_t glean_val_from_adcbuffer(int delay){
    // Gleans value from ADC buffer without changing the state of the buffer
    // Delay measured from current buffer index, 0, -1 one behind, -2 two behind etc
    
    // Make the algorithm periodic (protection for if gleaning more-than/equal length of the buffer)
    if(delay>=adc_bufferlen){
        int i=1;
        while(delay/(i*adc_bufferlen)!=0){i++;} // Integers always rounded down
        delay=delay-(i-1)*adc_bufferlen;
    }else if(delay<=-adc_bufferlen){
        int i=1;
        while(-delay/(i*adc_bufferlen)!=0){i++;}
        delay=delay+(i-1)*adc_bufferlen;
    }
    
    int ind=index_adcbuffer_oldest+delay; 

    if(ind<0){
        ind=adc_bufferlen+ind;
    }else if(ind>=adc_bufferlen){
        ind=ind-adc_bufferlen;
    }
    return adc_buffer[ind];
}

int16_t* unload_adc_buffer(int Nvals, int Nbpad, int Nfwpad)
{
    // Set the time of the oldest value loaded from the buffer, which is
    // adc_buffer_valcnt samples before the time of the latest sample 
    t_buffer.clk = t_latest_sample.clk - sampl2clk(adc_buffer_valcnt,Fs);

    // Glean Nbpad values, then unload Nvals from buffer, then glean Nfwpad vals
    // Pad with gleaned previous values
    int i; 
    int isave=0;
    int iglean=Nbpad;
//    printString("Here comes the gleaned value");printEL();
    for(i=0;i<Nbpad;i++)
    {
        adc_buffer_cpy[isave]=glean_val_from_adcbuffer(-iglean);
        isave++;
        iglean--;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen)
        {
            buffer_cpy_clip_flag=1; 
            break;
        }
    }

//    Unload values from ADC buffer
//    printString("Here comes the true adcbuffer value");printEL();
//    set_index_adcbuffer_oldest(0); // this needs to be removed, if sampling from adc, not from storage
   
    for(i=0;i<Nvals;i++){
        adc_buffer_cpy[isave]=read_val_from_adcbuffer();
        isave++;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
    
    // Pad with forward values
    iglean=0; // First value to glean forward is the next unloaded sample
    for(i=0;i<Nfwpad;i++){
        adc_buffer_cpy[isave]=glean_val_from_adcbuffer(iglean);
        isave++;
        iglean++;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
    return adc_buffer_cpy;
}

void fake_unload_adc_buffer(int Nframe)
{
    print_val_int(get_index_adcbuffer_oldest());
    print_val_int(get_adc_buffer_valcnt());

    int index = get_index_adcbuffer_oldest()+Nframe;
    int valcnt = get_adc_buffer_valcnt() - Nframe;
    
    if(index>=get_adc_buffer_len())
        set_index_adcbuffer_oldest(index % get_adc_buffer_len());
    else
        set_index_adcbuffer_oldest(index);
    if(valcnt < 0)
        set_adc_buffer_valcnt(0);
    else
        set_adc_buffer_valcnt(valcnt);
    
    print_val_int(get_index_adcbuffer_oldest());
    print_val_int(get_adc_buffer_valcnt());
}

int16_t* unload_adc_buffer_forget(int Nvals, int Nbpad, int Nfwpad)
{
    // Set the time of the oldest value loaded from the buffer, which is
    // adc_buffer_valcnt samples before the time of the latest sample 
    t_buffer.clk=t_latest_sample.clk - sampl2clk(adc_buffer_valcnt,Fs);

    // Glean Nbpad values, then unload Nvals from buffer, then glean Nfwpad vals
    // Pad with gleaned previous values
    int i; 
    int isave=0;
    int iglean=Nbpad;
//    printString("Here comes the gleaned value");printEL();
    for(i=0;i<Nbpad;i++)
    {
        adc_buffer_cpy[isave]=glean_val_from_adcbuffer(-iglean);
        isave++;
        iglean--;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen)
        {
            buffer_cpy_clip_flag=1; 
            break;
        }
    }

//    Unload values from ADC buffer
//    printString("Here comes the true adcbuffer value");printEL();
//    set_index_adcbuffer_oldest(0); // this needs to be removed, if sampling from adc, not from storage
    int index_oldest = get_index_adcbuffer_oldest();
    int valcnt = get_adc_buffer_valcnt();
    
    for(i=0;i<Nvals;i++){
        adc_buffer_cpy[isave]=read_val_from_adcbuffer();
        isave++;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
    set_index_adcbuffer_oldest(index_oldest);
    set_adc_buffer_valcnt(valcnt);
    
    // Pad with forward values
    iglean=0; // First value to glean forward is the next unloaded sample
    for(i=0;i<Nfwpad;i++){
        adc_buffer_cpy[isave]=glean_val_from_adcbuffer(iglean);
        isave++;
        iglean++;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
    return adc_buffer_cpy;
}

int16c* unload_adc_buffer_complex(int Nvals, int Nbpad, int Nfwpad){
    // Set the time of the oldest value loaded from the buffer, which is
    // adc_buffer_valcnt samples before the time of the latest sample 
    t_buffer.clk=t_latest_sample.clk - sampl2clk(adc_buffer_valcnt,Fs);
    
    printInt(adc_buffer_valcnt); printEL();
    printInt(sampl2clk(adc_buffer_valcnt, Fs)); printEL();
    
    // Glean Nbpad values, then unload Nvals from buffer, then glean Nfwpad vals
    // Pad with gleaned previous values
    int i; 
    int isave=0;
    int iglean=Nbpad;
    for(i=0;i<Nbpad;i++){
        adc_buffer_cpyc[isave].re=glean_val_from_adcbuffer(-iglean);
        adc_buffer_cpyc[isave].im=0;
        isave++;
        iglean--;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
        
    }
    // Unload values from ADC buffer
//    print_start_mark("ADC_BUFFER");
    for(i=0;i<Nvals;i++){
        adc_buffer_cpyc[isave].re = (int16_t)read_val_from_adcbuffer();
        adc_buffer_cpyc[isave].im = 0;
//        printInt(adc_buffer_cpyc[isave].re);printDelim();printEL();
        isave++; // to avoid zero in between
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
//    print_end_mark("ADC_BUFFER");

    // Pad with forward values
    iglean=0; // First value to glean forward is the next unloaded sample
    for(i=0;i<Nfwpad;i++){
        adc_buffer_cpyc[isave].re=glean_val_from_adcbuffer(iglean);
        adc_buffer_cpyc[isave].im=0;
        isave++;
        iglean++;
        // Break if clipping the buffer
        if(isave>=adc_bufferlen){
            buffer_cpy_clip_flag=1; 
            break;
        }
    }
    return adc_buffer_cpyc;
}

void test_unload_buffer_forget(void)
{
    cmnd_load_file();
    int Nframe = get_Nframe();
    
    transfer_data_from_storage_to_adc_buffer(Nframe);
    
    int16_t *p = unload_adc_buffer_forget(Nframe, 0, 0);
    int i;
    print_start_mark("FORGET");
    for(i = 0;i<Nframe;i++)
    {
        printInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("FORGET");

    print_val("index_oldest is ", get_index_adcbuffer_oldest());
    print_val("valcnt is ", get_adc_buffer_valcnt());
    
    int16_t *q = unload_adc_buffer(Nframe, 0, 0);
    print_start_mark("FULL");
    for(i = 0;i<Nframe;i++)
    {
        printInt(q[i]); printDelim(); printEL();
    }
    print_end_mark("FULL");

    print_val("index_oldest is ", get_index_adcbuffer_oldest());
    print_val("valcnt is ", get_adc_buffer_valcnt());
    
    p = unload_adc_buffer_forget(Nframe, 0, 0);
    print_start_mark("FORGET");
    for(i = 0;i<Nframe;i++)
    {
        printInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("FORGET");

    print_val("index_oldest is ", get_index_adcbuffer_oldest());
    print_val("valcnt is ", get_adc_buffer_valcnt());
    
    q = unload_adc_buffer(Nframe, 0, 0);
    print_start_mark("FULL");

    for(i = 0;i<Nframe;i++)
    {
        printInt(q[i]); printDelim(); printEL();
    }
    print_end_mark("FULL");

    print_val("index_oldest is ", get_index_adcbuffer_oldest());
    print_val("valcnt is ", get_adc_buffer_valcnt());
}

void load_file_downconvert_adc_buffer(void)
{
    ini_everything();
    long int fcenter=40000;
    long int B=5000;
    setup_downconversion(fcenter,B);

    cmnd_load_file();

    int i;
    int N_input = get_load_num();
    int Nframe = get_Nframe();
    int Nframes = (int) N_input / Nframe;
    int Nfwpad_dcfilt = get_Nfwpad_dcfilt();

    Nframes++;

    while(Nframes--)
    {
        for (i=0;i<Nframe+Nfwpad_dcfilt;i++)
            insert_val_into_adcbuffer((int16_t)read_from_storage());
        downconvert_adc_buffer(Nframe);
        print_adc_buffer_dc();
    }
    transmit_storage_vector_test();
}

void transfer_data_from_storage_to_adc_buffer(int Nframe)
{
    int i;
    for (i = 0;i<Nframe;i++)
    {
        insert_val_into_adcbuffer((int16_t) read_from_storage());
    }
}

void transfer_data_from_adc_buffer_to_storage(int Nframe)
{
    acquire_samples(Nframe); // wait until i have Nframe samples inside adc buffer
    int i;
    int16_t * p = unload_adc_buffer(Nframe, 0, 0);
    for (i = 0;i<Nframe;i++)
        insert_into_storage((int32_t)p[i]);
}

void print_adc_buffer(void)
{
    int i, Nsample;
    Nsample = get_adc_buffer_len();
    int16_t * p = get_adc_buffer();
    print_start_mark("ADC_BUFFER");
    for (i = 0;i<Nsample;i++)
    {
        printInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("ADC_BUFFER");
}

void print_adc_buffer_Nframe(int Nframe)
{
    int i;
    int16_t * p = unload_adc_buffer(Nframe,0, 0);
//    int16_t *p = get_adc_buffer();
    print_start_mark("ADC_BUFFER");
    for (i = 0;i<Nframe;i++)
    {
        printInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("ADC_BUFFER");
}

void print_adc_buffer_dc(void)
{
    int i, Ndc;
    Ndc = get_Ndc_baseband();
    int32c * p = get_dc_vals_lpfilt();
    print_start_mark("DC_BUFFER");
//    for (i = 0;i<Ndc;i++)
//    {
//        printLongInt((int32_t) p[i].re * p[i].re + p[i].im * p[i].im); printDelim(); printEL();
//    }
    for (i = 0;i<Ndc;i++)
    {
        printLongInt((int32_t) p[i].re ); printDelim(); printEL();
        printLongInt((int32_t) p[i].im ); printDelim(); printEL();
    }
    print_end_mark("DC_BUFFER");
}

void print_noisevar_buffer(void)
{
    int i, Nbuffer;
    Nbuffer = get_threshold_buffer_len();
    int32_t *p = get_noisevar_buffer();
    print_start_mark("NOISE_BUFFER");
    for (i = 0;i<Nbuffer;i++)
    {
        printLongInt((int32_t) p[i] ); printDelim(); printEL();
    }
    print_end_mark("NOISE_BUFFER");
}

void print_val(char *p, int val)
{
//    print_start_mark(p);
    printString(p); printString(" is "); 
    printInt(val); printDelim(); printEL();
//    printDouble(val); printDelim(); printEL();
//    print_end_mark(p);
}

void print_val_double(double val)
{
    printDouble(val); printDelim(); printEL();
}

void print_val_int(int val)
{
    printInt(val); printDelim(); printEL();
}

void print_adc_buffer_valcnt(void)
{
//    printString("adc buffer valcnt is "); 
    printInt(get_adc_buffer_valcnt()); printDelim(); printEL();
}

void ini_everything(void)
{
    clear_buffer();
    clear_storage();
    clear_storage_test();
    clear_buffer_dc();
}

void ini_ind_adc_buffer(void)
{
//    int Nsample = get_adc_buffer_len();
    set_index_adcbuffer_oldest(0);
//    set_index_adcbuffer_nextsmpl(Nsample);
}

void print_ind_o(void)
{
    print_start_mark("ADC_BUFFER_INDICES");
    int adc_buffer_ind_oldest = get_index_adcbuffer_oldest();
    int adc_buffer_ind_nextsmpl = get_index_adcbuffer_nextsmpl();
    printString("Oldest sample is "); printInt(adc_buffer_ind_oldest); printEL();
    printString("Next sample is "); printInt(adc_buffer_ind_nextsmpl); printEL();
    printString("adc_buffer_valcnt is "); printInt(get_adc_buffer_valcnt()); printEL();
    print_end_mark("ADC_BUFFER_INDICES");
}

void test_pre_filtering_with_fixed_threshold(int Nframes)
{
    print_val("fixed_threshold is ", fixed_threshold);
    pre_filter_fixed_threshold_detector(Nframes);
}

void test_pre_filtering_with_adapted_threshold(int Nframes)
{
    ini_threshold_buffer();
    printString("NOISE LEVEL BUFFER IS SET UP SUCCESSFULLY!"); printEL();
    pre_filter_adapted_threshold_detector(Nframes);
}

void print_start_mark(char *p)
{
//        printString("==========================================================\\"
//        "===============================================================\\"
//        "==========================================");printEL();
        printString("##START_"); printString(p); printString("##"); printEL();
}

void print_end_mark(char *p)
{
        printString("##END_"); printString(p); printString("##"); printEL();
//        printString("==========================================================\\"
//        "===============================================================\\"
//        "==========================================");printEL();
}

void save_adc_buffer_dc_to_storage(void)
{
    int N = get_Ndc_baseband();
    int32c * p = get_dc_vals_lpfilt();
    int i;
    for (i=0;i<N;i++)
        insert_into_storage_dc(abs_32c(p[i]));
}

// Set all values in the storage to zero, and clear read/write indices
void clear_buffer(void){
    int i;
    for(i=0;i<adc_bufferlen;i++){
        adc_buffer[i]=0;
    }
    // Reset index
    index_adcbuffer_nextsmpl=0;
    index_adcbuffer_oldest=0;
    adc_buffer_valcnt = 0;
}

// Set all values in the storage to zero, and clear read/write indices
void clear_buffer_dc(void){
    int i;
    
    for(i=0;i<get_adc_buffer_dc_len();i++){
        adc_buffer_dc[i].re=0;
        adc_buffer_dc[i].im=0;
    }
    // Reset index
    index_adcbuffer_nextsmpl=0;
}

void clear_raw_data_buffer(void)
{
    int i;
    for(i=0;i<raw_data_buffer_len;i++){
        raw_data[i]=0;
    }
    // Reset index
    index_raw_data_buffer=0;
    raw_data_buffer_valcnt = 0;
}

// --------------------------------------------------
// Algorithm timeout functions ------------------------
void set_algorithm_timeout(double at){
    timeout_alg.clk=sec2clk(at);
}
void reset_algorithm_timeout(void){
    t0_alg.clk=get_time_clk();
}
char check_algorithm_timeout(void){
    if ( timeout_alg.clk < get_time_clk()-t0_alg.clk ) { // t0_alg is the true time of arrival      
        // Timeout from no echo
        IEC0bits.T2IE = 0; // Disable interrupt for Timer2
        T2CONbits.ON = 0; //stop pulse out timer
        printMessage(MSG_TIMEOUT_DETECTION);
        return TRUE; // Timeout
    }else{
        return FALSE; // No timeout
    }
}

// Downconversion & lp filter ---------------

double get_time_diff(Time t1, Time t2)
{
    return clk2sec(t2.clk - t1.clk);
}

int get_fixed_threshold(void)
{
    return fixed_threshold;
}

int get_fixed_threshold_dc(void)
{
    return fixed_threshold_dc;
}

int get_Nbpad_dcfilt(void){
    return Nbpad_dcfilt;
}
int get_Nfwpad_dcfilt(void){
    return Nfwpad_dcfilt;
}
int get_Ndc_baseband(void){
    return Ndc_baseband;
}
int get_Ndc_passband(void){
    return Ndc_passband;
}
int get_dc_factor(void){
    return dc_factor;
}
int get_Ntaps(void){
    return Ntaps_5kHz;
}

int32c * get_lpfilt(void)
{
    return lpfilt;
}

// Buffers
int get_buffer_clip_flag(void){
    return buffer_cpy_clip_flag;
}
int16_t* get_adc_buffer(void) {
    return adc_buffer;
}
int get_adc_buffer_len(void) {
    return adc_bufferlen;
}
int16c* get_adc_buffer_dc(void){
    return adc_buffer_dc;
}

int get_adc_buffer_dc_len(void) {
    return adc_bufferlen_dc;
}

int get_Nframe(void)
{
    return Nframe;
}

void set_Ndc_baseband(int Nvals)
{
    Ndc_baseband = Nvals;
}

void set_Ndc_passband(int Nvals)
{
    Ndc_passband = Nvals;
}

int32c* get_dc_vals_lpfilt(void){
    return dc_vals_lpfilt;
}

int32c * get_dc_vals(void){
    return dc_vals;
}

int16c* get_dc_exponential(void){
    return dc_exponential;
}

// Noise estimation -------------
int32_t* get_noisevar_buffer(void){
    return threshold_buffer;
}
int get_threshold_buffer_valcnt(void){
    return threshold_buffer_valcnt;
}

// Temporary - will be removed ----------
int get_index_adcbuffer_nextsmpl(void){
    return index_adcbuffer_nextsmpl;
}
int get_index_adcbuffer_oldest(void){
    return index_adcbuffer_oldest;
}

int get_index_noise_level_buffer(void){
    return index_threshold_buffer;
}

void set_index_threshold_buffer(int index){
    index_threshold_buffer = index;
}

//int32c* get_buff_unf(void) { //holds input template for FFT in Q15 format
//    return buff_unf;
//}
//int32c* get_buff_unf_freq(void) { //holds output template from FFT in Q15 format
//    return buff_unf_freq;
//}
//  ------------------

int get_adc_buffer_valcnt(void){
    return adc_buffer_valcnt;
}
void set_adc_buffer_valcnt(int cnt){
    adc_buffer_valcnt=cnt;
}

// Sampling
void set_sampling_rate(long int Fs_){
    Fs=Fs_;
    // Configure sampling timer so that it will sample at Fs [Hz]
    if ((Fs > FCY / 0xFFFF) && (Fs < FCY)) {
        // Sampling close to MCU clock is risky because depending on the number 
        // of clock cycles the interrupt takes, it might not be able to finish 
        // before the next interrupt.
        PR2 = (int) ((float) FCY / Fs);
    } else {
        printString("Can not sample lower than 0xFFFF Hz by only using the "
                "period register PR2 for Timer2. Implement a function (here) "
                "using the prescaler register "
                "T2CONbits.TCKPS = 0bXXX; !");
        printEL();
    }
}

char get_new_sampl(void)
{
    return new_sampl;
}

Time get_t_buffer(void)
{
    return t_buffer;
}

// ===================== YL functions ======================================

void print_lpfilt(void)
{
    int i = 0;
    for (i = 0;i<Ntaps_5kHz;i++)
    {
        printInt(lpfilt[i].re); printEL();
    }
    printString("low pass filter has been printed");
}

void print_dc_component(int Ncpy)
{
    print_start_mark("DC_COMPONENT");
    int n = 0;
//    for(n=0;n<dc_vecs_len;n++){
    for(n=0;n<Ncpy;n++){
        printInt(dc_exponential[n].re);printDelim();printEL();
        printInt(dc_exponential[n].im);printDelim();printEL();
    }
    print_end_mark("DC_COMPONENT");
}

void print_dc_vals(int Ncpy)
{
    print_start_mark("DC_VALS");
    int n = 0;
    for(n=0;n<Ncpy;n++){
        printLongInt(dc_vals[n].re);printDelim();printEL();
        printLongInt(dc_vals[n].im);printDelim();printEL();
    }
    print_end_mark("DC_VALS");
}

void print_dc_vals_lpfilt(void)
{
    print_start_mark("DC_VALS_LPFILT");
    int32c * p = get_dc_vals_lpfilt();
    int n;
    for (n=0;n<get_Ndc_passband();n++)
    {
        printLongInt(p[n].re);printDelim();printEL();
        printLongInt(p[n].im);printDelim();printEL();
    }
    print_end_mark("DC_VALS_LPFILT");
}

int get_Ncpy(void)
{
    int Ndc_passband = get_Ndc_passband();
    int Ncpy = Nbpad_dcfilt+Ndc_passband+Nfwpad_dcfilt;
    return Ncpy;
}

void set_index_adcbuffer_oldest(int N)
{
    index_adcbuffer_oldest = N;
}

void set_index_adcbuffer_nextsmpl(int N)
{
    index_adcbuffer_nextsmpl = N;
}

int get_scale_factor(void)
{
    return scale_factor;
}

int get_exceedance(void)
{
    return exceedance;
}

void set_exceedance(int val)
{
    exceedance = val;
}