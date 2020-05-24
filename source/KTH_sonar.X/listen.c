#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include <stdint.h> 
#include <math.h>
#include "listen.h"
#include "memory.h"
#include "downlink.h"
#include "delay.h"
#include "detection.h"
#include "SONARconfig.h"
#include "sonar.h"
#include "pulse.h"
#include "sampling.h"

float noiserms = 0;
int mean_noise = 0;
double standard_deviation = 0;

// Function for sampling as quickly as possible. Is used when doing raw 
// listening after sending a pulse (without detector)
void raw_listen( int16_t *raw_ADC, int raw_ADC_len ){

    setTransducerToSensing();
    int i;    
    ADCCON3bits.VREFSEL = 1;
    ADCCON1bits.ON = 1;
    ADCCSS1bits.CSS15 = 0; //AN15 (Class 3) quit scanning
    ADCCSS1bits.CSS18 = 1; //AN18 (Class 3) set for scan
    ADCCON3bits.GSWTRG = 1;
    int t_pulsestart = get_t_pulsestart();
    int t_startdetector=TMR4; 

    for (i=0; i<raw_ADC_len; i++){
        raw_ADC[i] = ADCDATA18;
        ADCCON3bits.GSWTRG = 1;
    }   
    set_deadtime((double)(t_startdetector-t_pulsestart)/FCY);
}

void listen_full_storage(void)
{
    start_sampling((long int)DEFAULT_SAMPLING_FREQUENCY,.5);
    int N = get_storage_vector_len();
//    int Nsample = get_adc_buffer_len();
//    int Nframe = Nsample / 2;

    int Nframe = get_Nframe();

    int Nframes = (int) N/Nframe; // number of frames
    int Nresidue = N - Nframe * Nframes;  // fulfill the entire storage vector
    int i;
    while(Nframes--)
    {
        acquire_samples(Nframe); // wait until i have Nframe samples inside adc buffer
        int16_t* p=unload_adc_buffer(Nframe,0,0);
        for (i=0;i<Nframe;i++) 
            insert_into_storage( (int32_t) p[i] ); // normal value
    }
    acquire_samples(Nresidue);
//    print_val("Nresidue is ", Nresidue);
    int16_t *p = unload_adc_buffer(Nresidue, 0, 0);
    for (i = 0;i<Nresidue;i++)
        insert_into_storage((int32_t)p[i]);

    transmit_storage_vector();    
    stop_sampling();
}

void listen_N_samples(int N)
{
    start_sampling((long int)DEFAULT_SAMPLING_FREQUENCY,.5);
    acquire_samples(N); // wait until i have Nframe samples inside adc buffer
    int16_t* p=unload_adc_buffer(N,0,0);
    int i;
    for (i=0;i<N;i++) 
        insert_into_storage( (int32_t) p[i] ); // normal value

    transmit_storage_vector_N(N);    
    stop_sampling();
}

void listen_for_downconversion(int Nframes)
{
    start_sampling((long int)DEFAULT_SAMPLING_FREQUENCY,.5);
    int Nsample = get_adc_buffer_len();
    int Nframe = Nsample / 2;

    while(Nframes--)
        transfer_data_from_adc_buffer_to_storage(Nframe);
    transmit_storage_vector();
    stop_sampling();
}

void listen(int N)
{
    clear_storage();
    start_sampling((long int)DEFAULT_SAMPLING_FREQUENCY,.5);
    int i,j;
//    int N = get_storage_vector_len();
    int Nframe=get_adc_buffer_len();
    int Nframes = N/Nframe;
    if (Nframes*Nframe > N)
        Nframes--;
    i = 0;
    while(i<=Nframes)
    {
        acquire_samples(Nframe); // wait until i have Nframe samples inside adc buffer
        int16_t* p=unload_adc_buffer(Nframe,0,0);
        for (j=0;j<Nframe;j++)
        {
            insert_into_storage( (int32_t) p[j] ); // normal value
        }
        i++;
    }
    transmit_storage_vector();
    stop_sampling();
//    start_sampling((long int)DEFAULT_SAMPLING_FREQUENCY,.5);
//    loop 20 times
//    set_storage_rindex(0);
//    set_storage_windex(0);

//            printInt(get_storage_windex()); printDelim();
//            printEL();
    //        set_storage_windex(get_storage_windex()+1);
//            printLongInt(p[j]); printDelim(); printEL();
//        printInt(i); printEL();

//    transmit_storage_vector_N(); // Uncomment to download storage vector

//        printString("The current index is");
//            printInt(i); printEL();
//            printInt(get_storage_windex()); printEL();
}

// Sample background noise and determine its standard deviation and mean.
void listen_noise( void ) {//sample noise and calculate noise rms

    // 400 seems to be enough to consquently get similar standard  
    // deviation for similar noise levels
    int noise_buff_len = 1000;   
    int noise_buff [noise_buff_len];
    int i;
    volatile double sigma_sq = 0;
    double sigma = 0;
    
    memset(noise_buff, 0, sizeof(int)*noise_buff_len);
    double mn = 0; // Mean noise
    
    setTransducerToSensing();
    //delaymicro(MIN_INPUTSWITCH_DELAY);
    ADCCON3bits.VREFSEL = 1;
    ADCCON1bits.ON = 1;
    ADCCSS1bits.CSS15 = 0; //AN15 (Class 3) quit scanning
    ADCCSS1bits.CSS18 = 1; //AN18 (Class 3) set for scan
    ADCCON3bits.GSWTRG = 1;

    for (i=0; i<noise_buff_len; i++){
        while (ADCDSTAT1bits.ARDY18 == 0);
        noise_buff[i] = ADCDATA18;

        mn = (double) noise_buff[i] + mn;
        ADCCON3bits.GSWTRG = 1;
    }
    
    mn = mn/noise_buff_len;

    // For some reason the first few sampled noise values are slightly 
    // offset, especially the first time sampling occurs after power cycling
    // (maybe the op-amp needs to warm up?). This severely affects the 
    // standard deviation calculation, which is why the first few (~10 are 
    // enough to avoid this) values are 
    // overwritten with the mean of the noise vector.
    for (i=0; i<10; i++){
        noise_buff[i] = mn;
    }
    
    for (i=0; i<noise_buff_len; i++){
        sigma_sq = sigma_sq + pow( noise_buff[i] - mn, 2 );
    }
    sigma_sq = sigma_sq/( noise_buff_len - 1 ); // /(N-1)
    sigma = sqrt(sigma_sq);
    
    standard_deviation = sigma;
    mean_noise = (int) mn;

}

// Getters and setters for listen variables
float get_noiserms(void){ return noiserms; }
void set_mean_noise(int mean){ mean_noise = mean; }
int get_mean_noise(void){ return mean_noise; }
double get_standard_deviation(void){ return standard_deviation; }
