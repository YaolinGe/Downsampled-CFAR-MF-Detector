#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <stdint.h>     // Necessary for int16_t declaration
#include <math.h>
#include <string.h>
#include "main.h"
#include "detection.h"
#include "delay.h"
#include "sonar.h"
#include "pulse.h"
#include "listen.h"
#include "memory.h"
#include "modes.h"
#include "flags.h"
#include "downlink.h"
#include "messages.h"
#include "sonar_math.h"
#include "sampling.h"
#include "SONARconfig.h"

// yaolin debug added
#include "commands.h" // contained to use to upload file commands, can be deleted later
#include "time.h"
#include<limits.h>

// Keep track of some performance indicators of the sonar
double detector_extime=0;   // Time to process one frame
double t_frame=0;           // Time to load one frame, 2^NFFT-Npulse

// Note: Switching electronics to take input from the transducer 
// introduces disturbances on the adc from the board, waiting 
// 300 us "removes" this from the adc data, and allows the 
// detector to be made more sensitive.

double last_sound_pressure=-1;

//int raw_adc_len = RAW_ADC_LEN_MAX;
double last_recorded_depth = -1;
double depth_min=-1;
double depth_max=-1;
long int last_recorded_depth_correlation=0;
int np = DEFAULT_PULSENUM;
int frequency = DEFAULT_RESONANCE_FREQUENCY;// Frequency on output wave
int bandwidth=5000; // for sinuousidal signal, for chirp is 6000
volatile int voltage = DEFAULT_VOLTAGE;     // Active voltage on sonar output
double max_range = DEFAULT_MAX_RANGE;
int threshold = 2340; // set manual threshold



/* sonar.c ************************************************
 * This file contains functions that pertain to the operation 
 * of the sonar, i.e. listening to the background noise, looking
 * for an echo, and what pulse functions to use with the current 
 * operation mode. 
 */


/****************************************************************
 * Operate the sonar according to operation modes set by commands;
 * e.g. send specific types of pulses, output data in certain formats
 * etc.
 */
void operateSonar(void) {

    // Note: transducer set to sensing in listen(), 
    // and set to pulsing in send_pulsetrain,
    // but is not set to sensing inside determineEcho()

    //operateVoltageTracking();

    // Reset clock
    TMR4 = 0;

    // If running debug mode, the sonar is sampling echos from
    // the storage. Reset the read index of the storage 
    // (so that the sonar reads from the beginning each time).
    if (get_Mode_debug().state == TRUE) {
        // Set start delay to zero
    } else {
        //set_startdelay(DEFAULT_STARTDELAY);
    }

    // Run code that the programmer wants to use with a debug command
    //if (get_Mode_debug().state == 1) {
    if (get_Flag_cmd_debug().state == TRUE) 
    {
// ===================== downconversion function ===============================
//        printString("debug receiver mode is on"); printEL();
// ++++++++++++++++ SYSTEM SETUP 0525 _++++++++++++++++++++++++++++++++++++
        if(current_device == DEVICE_BEACON)
        {
            printString("beacon is running on this device, good luck!"); printEL();
            beacon();
        }
        else if(current_device == DEVICE_TRANSMITTER)
        {
            printString("transmitter is running on this device, good luck!"); printEL();
            transmitter();
        }
        
        
        
//+++++++++++++++++++++ DEBUG 0513 +++++++++++++++++++++++
//        debug_centre_frequency_recalibration();
//        debug_ramping_up_delay_calibration();
//        debug_start_delay_recalibration();
//        debug_beacon();
//        beacon();
//        beacon_2nd();
//        test_interaction();
//        debug_transmitter();
//        transmitter();
//        transmit_storage_vector();
//        transmit_storage_vector_test();
//        print_threshold_buffer();
//+++++++++++++++++ END DEBUG 0513 +++++++++++++++++++++++
//        print_start_mark("CHARGE_100");
//        int i;
//        for (i = 0;i<100;i++)
//            send_square_pulse(10, 40000, 100);
//        print_end_mark("CHARGE_100");
//        
//        print_start_mark("CHARGE_200");
//        for (i = 0;i<100;i++)
//            send_square_pulse(10, 40000, 200);
//        print_end_mark("CHARGE_200");
//        
//        print_start_mark("CHARGE_300");
//        for (i = 0;i<100;i++)
//            send_square_pulse(10, 40000, 300);
//        print_end_mark("CHARGE_300");
//        
//        print_start_mark("CHARGE_400");
//        for (i = 0;i<100;i++)
//            send_square_pulse(10, 40000, 400);
//        print_end_mark("CHARGE_400");
        
// ++++++++++++++++++++ DELAY COMPARISON ++++++++++++++++++++++++++++++++++++
//        Time t1;
//        Time t2;
//        t1.clk = get_time_clk();
////        delaymicro_calibrated(1000000);
////        delaymicrosec(1);
//        delay_clk(100);
//        t2.clk = get_time_clk();
//        printString("the delayed time is "); printInt(t2.clk - t1.clk); printString("clks"); printEL();
//        t1.clk = get_time_clk();
////        delaymicro_calibrated(1000000);
//        delaymicrosec(1000000);
//        t2.clk = get_time_clk();
//        printString("the delayed time is "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();     
//        t1.clk = get_time_clk();
//        delaymicro_calibrated(1000000);
////        delaymicrosec(1);
//        t2.clk = get_time_clk();
//        printString("the delayed time is "); printDouble(get_time_diff(t1, t2)); printString("secs"); printEL();   
// ++++++++++++++++++++ DELAY COMPARISON ++++++++++++++++++++++++++++++++++++


//// ++++++++++++++++++++++++ TEST FUNCTIONS +++++++++++++++++++++++++++++++++++++
        
//        int Nframes = 1000;
//        int i;
//        start_sampling(get_Fs(), .5);
//        print_start_mark("VALCNT");
//        while(Nframes--)
//        {
////            TMR4 = 0;
//            int flag_save = FALSE;
//            acquire_samples(2112 + 3);
//            print_val_int(get_adc_buffer_valcnt());
//            int16c *p = unload_adc_buffer_complex(2112, 3, 3);
//            
//            print_val_int(get_adc_buffer_valcnt());
//
//            for (i = 0;i<2118;i++)
//                if (p[i].re>2380)
//                {
//                    flag_save = TRUE;
//                    break;
//                }
//            
//            if(flag_save)
//            {
//                stop_sampling();
//                for(i = 0;i<2118;i++)
//                    insert_raw_data_into_buffer(p[i].re);
//                print_end_mark("VALCNT");
//
////                print_adc_buffer();
////                print_raw_data_buffer();
////                    insert_into_storage(p[i].re);
//                start_sampling(get_Fs(), .5);
//                delaymilli(500);
////                return;
//            }
//        }
//        print_raw_data_buffer();
//        print_adc_buffer();
//        print_threshold_buffer();
        
        
        
//        transmit_storage_vector();
        
        
        
//        int32_t a[10] = {1,2,3,4,10,3,2,4,5,2};
////        int ind = max_index_array_int32_t(a, 10);
//        int ind = first_exceedance_index_array_int32_t(a, 10, 3);
//        printString("the max indes is "); printInt(ind); printEL();
//        debug_overflow();
//        debug_clk_speed_tracking();
//        test_valcnt_calibration();
        
//        test_load_file_downconvert_adc_buffer();
//        test_pulse_listen_downconvert_adc_buffer();
//        test_thresholding();
//        test_noise();
//        test_listen_downconvert();
//        test_unload_buffer_forget();
//        test_load_file_matched_filtering();
//        int Nframes = 1000;
//        test_pre_filtering_with_fixed_threshold(Nframes);
//        load_file_matched_filter_corr();
//        load_file_matched_filter_thresholding();
        
//        test_ringing_effect_bleeding();
        
//        print_val("The maximum limit of integer is ", INT_MAX);
//        test_matched_filtering();
//         test_signal_generation();
//        int Nframe = NFFT;
//        ini_noise_level_buffer();
        
//        test_env_sampling();
//        test_time_calculation();
//            beacon();
//        transmitter();
//            test_send_pulse();

// ++++++++++++++++++++++++ TEST FUNCTIONS +++++++++++++++++++++++++++++++++++++
        printString("end of debug mode!"); printEL();
    }
    if (get_Flag_cmd_debug_r().state == TRUE)
    {
        send_pulse(40000, SINE, 0);
    }
    // Perform a pulse with raw listening
    if (get_Flag_cmd_send_pulse().state == TRUE) {

        set_storage_rindex(0);
        set_storage_windex(0);
        double timeout=2*(get_max_range())/get_soundspeed();
        // Set up the matched filter
//        listen(2*NFFT,FALSE);  // Initiate buffer with bias values

//        setup_MatchedFilter(timeout,get_wave_select());
        print_val("frequency", get_frequency());
        if(get_wave_select()==SINE){
            send_square_pulse(np, frequency, voltage);
        } else if(get_wave_select()==CHIRP){
            send_chirp_pulse(np,voltage,frequency,bandwidth);
        } 
//        detectEchoMatchedFilter();
        printMessage(MSG_DEPTH);

    } else if (get_Flag_cmd_send_pulse_listen().state == TRUE) {

        ini_everything();
        send_pulse(40000, SINE, 0);
//        set_wave_select(SINE);
//        set_frequency(47000);
//        set_startdelay(6000);
//        if(get_wave_select()==SINE){
//            send_square_pulse(np, frequency, voltage);
//        } else if(get_wave_select()==CHIRP){
//            send_chirp_pulse(np,voltage,frequency,bandwidth);
//        }
        
        Time tlisten;
        Time tpulse;
        tpulse.clk=get_t_pulsestart();
        tlisten.clk=get_time_clk();
        listen_full_storage(); // listen to fulfill the entire storage vector
//        listen_N_samples(1000);
        printEL();
        printString("Time elapsed is "); printDouble(clk2sec(tlisten.clk-tpulse.clk));printEL();

//        printEL();

//        int post_pulse_delay = 0;
//        int post_pulse_delay = get_startdelay();
//        delaymilli(post_pulse_delay);
//        int N = get_storage_vector_len();
//        listen(N);

//        int std = get_startdelay();
//        listen(2*NFFT,FALSE);

//        send_square_pulse(100, 45000, 250);
//        send_square_pulse(200, 1000, 2);

        printString("Done listening/correlating");
        printEL();

    } else if (get_Flag_cmd_send_pulse_listenc().state == TRUE) {

        set_storage_rindex(0);
        set_storage_windex(0);
        double timeout=2*(get_max_range())/get_soundspeed();
        // Set up the matched filter
        setup_MatchedFilter(timeout,get_wave_select());
        //listen(2*NFFT,FALSE);
        if(get_wave_select()==SINE){
            send_square_pulse(np, frequency, voltage);
        } else if(get_wave_select()==CHIRP){
            send_chirp_pulse(np,voltage,frequency,bandwidth);
        }        
//        listenc(get_storage_vector_len());
        printString("Done listening/correlating");
        printEL();
        
    //    set_startdelay(std);
    } else if (get_Mode_detect_waveform().state == TRUE 
            && get_Mode_autoping().state == FALSE ){
        // Only detect 70 kHz
        double freq=get_frequency();
//        int thrn=get_thrscale_nom();
//        int thrrev=get_thrscale_reverb();
        
    //    set_thrscale_nom(1);
    //    set_thrscale_reverb(1);
        //int Fs=get_Fs();
        //set_frequency(70000);
        //set_Fs(180000);
        set_np(10);
        //if(get_thrscale_nom()<2){set_thrscale_nom(2);}
//        set_minthresh(10);
        // 
        //set_thrscale_nom(4);
        set_storage_rindex(0);          // Read from beginning in memory
        set_storage_windex(0);          // Read from beginning in memory

        //listen(2*NFFT,FALSE);  // Initiate buffer with bias values
        setup_MatchedFilter(6.0,SINE);
    //    detectWaveform();

//        set_thrscale_nom(thrn);
//        set_thrscale_reverb(thrrev);
        
        if(get_last_sound_pressure()==0){
            printString("-"); printEL();
        }
        else{
            printDouble(get_last_sound_pressure()); printEL();
        }
        // Reset parameters
        set_frequency(freq);
//        set_thrscale_nom(thrn); 
        delaymilli(100);
        //set_Mode_detect_waveform(FALSE);
    }
}

void operateVoltageTracking(void){
   // Set voltage automatically 
    // (NEED TO CHANGE SO THAT I DONT USE PULSE TRAIN FLAG)
    if (get_Mode_autoset_voltage().state == TRUE 
            && get_Flag_cmd_send_pulse().state==TRUE) {
   
        // Got no hit, ramp up voltage
        if (get_Mode_ramping_up().state == TRUE && get_consecutive_hits() == 0) {
            voltage = voltage + 20;
        }
        // Got a hit, raise voltage level one more time to get some margin
        if (get_Mode_ramping_up().state == TRUE && get_consecutive_hits() >= 1) {
            set_Mode_ramping_up(FALSE);
            voltage = voltage + 20;
        }
        
        // Lost tracking, do voltage ramp up again
        if (get_Mode_ramping_up().state == FALSE && get_consecutive_misses() >= 5) {
            // dump_voltage(); // This needs to be tested
           set_consecutive_hits(0);
           set_consecutive_misses(0);
           set_Mode_ramping_up(TRUE);
           set_Mode_autoset_voltage(TRUE);
           //set_voltage(MIN_VOLTAGE); // Start ramping up from V=min_voltage
        }
        // Reached max voltage, stop ramping up
        if (get_Mode_ramping_up().state == TRUE && voltage >= MAX_VOLTAGE) {
            voltage = MAX_VOLTAGE;
            set_Mode_ramping_up(FALSE);
        }
        
        // If within saturation region, ramp down
        if(get_Mode_ramping_down().state==TRUE){
            voltage=voltage-20;
        }
        
        // Stop ramping down if at minimum voltage
        if(get_Mode_ramping_down().state == TRUE && voltage<=MIN_VOLTAGE){
            voltage=MIN_VOLTAGE;
            set_Mode_ramping_down(FALSE);
        }
        
        // Start ramping down if saturation in the detector is flagged
        if(get_Flag_saturation().state==TRUE){
            set_Mode_ramping_down(TRUE);
        }
    }
}

/****************************************************************
 **************** Getters and setters ****************************
 ****************************************************************/

void set_np(int p) {
    np = p;
}

void set_frequency(float freq)
{
    frequency=freq;
    int rerr = 0;
    int magic_freq_compensator=96;
    // magic_freq_compensator compensates the frequency to the correct value 
    // for the 2*216 nops() delay in the pulse switching, which is there to 
    // avoid shot through (current that rushes through the transistors 
    // as they are switching from high to low)
    rerr = ((FCY / freq) / 2) - magic_freq_compensator;
    //rerr = ((150000000 / freq) / 2) - 27;
    PR1 = rerr;
    Nop();
}

void set_fixed_threshold(int32_t threshold)
{
    fixed_threshold = threshold;
    Nop();
}

void set_scale_factor(int scale)
{
    scale_factor = scale;
    Nop();
}

int get_fixed_threshold(void) {
    return fixed_threshold;
}

int get_np(void) {
    return np;
}

int get_voltage(void) {
    return voltage;
}
void set_voltage(int v) {
    voltage = v;
}
float get_frequency(void) {
    return frequency;
}
double get_last_recorded_depth(void) {
    return last_recorded_depth;
}
void set_last_recorded_depth(double d) {
    last_recorded_depth = d;
}
long int get_last_recorded_depth_correlation(void) {
    return last_recorded_depth_correlation;
}
void set_last_recorded_depth_correlation(long int d) {
    last_recorded_depth_correlation = d;
}

double get_depth_min(void){
    return depth_min;
}

double get_depth_max(void){
    return depth_max;
}

void set_depth_min(double dmin){
    depth_min=dmin;
}

void set_depth_max(double dmax){
    depth_max=dmax;
}

void set_last_sound_pressure(double sp){
    last_sound_pressure=sp;
}

double get_last_sound_pressure(void){
    return last_sound_pressure;
}

double get_t_frame(void){
    return t_frame;
}

double get_detector_extime(void){
    return detector_extime;
}

void set_t_frame(double tf){
    t_frame=tf;
}

void set_detector_extime(double detext){
    detector_extime=detext;
}

double get_max_range(void){
    return max_range;
}

int get_bandwidth(void){
    return bandwidth;
}

void set_bandwidth(int bw){
    bandwidth=bw;
}

// yaolin debugfunction
int get_center_freq(void){
    return DEFAULT_RESONANCE_FREQUENCY;
}