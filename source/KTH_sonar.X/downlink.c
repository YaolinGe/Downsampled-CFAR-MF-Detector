#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include <stdint.h>
#include "delay.h"
#include "downlink.h"
#include "sonar.h"
#include "flags.h"
#include "UART.h"
#include "modes.h"
#include "pulse.h"
#include "detection.h"
#include "SONARconfig.h"
#include "math.h"
#include "memory.h"
#include "detection.h"
#include "sampling.h"


#define DEFAULT_BUFFERSIZE 200

// Delay settings
volatile int comdelay = 0;
char downlinkbuffer[DEFAULT_BUFFERSIZE]; //buffer for UART output
/* downlink.c ************************************************
 * This function contains generic downlink functions, that are an
 * interface to io.c, and also downlink functions that send a 
 * package of data (like a package of variables). The generic 
 * downlink functions use specific functions for the chosen 
 * downlink pipe, e.g. UART, and the pipe is chosen by setting 
 * the "downlink_mode_" variables. The generic functions need 
 * if-cases to handle downlinking to different pipes.
 */

/****************************************************************
 ******** Interface between hardware IO communication and   ******
 ******** software IO printing (printing stuff in the code) ******
 ****************************************************************/
void printString(char *string) {
    transmitStringUART(downlinkbuffer, string);
}

void printIntArray(int *arr, int arr_len) {
    int count = 0;
    do {
        transmitIntUART(downlinkbuffer, arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printLongIntArray(long int *arr, int arr_len) {
    int count = 0;
    do {
        transmitLongIntUART(downlinkbuffer, arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printInt16_tArray(int16_t *arr, int arr_len) {
    int count = 0;
    do {
        transmitIntUART(downlinkbuffer, (int)arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printDoubleArray(const double *arr, int arr_len) {
    int count = 0;
    do {
        transmitDoubleUART(downlinkbuffer, arr[count]);
        transmitEndLineUART(comdelay);
        count++;
    } while (count < arr_len);
}

void printInt(int i) {
    transmitIntUART(downlinkbuffer, i);
}

void printInt16c_delim(int16c val){
    transmitIntUART(downlinkbuffer, val.re);  printDelim();
    transmitIntUART(downlinkbuffer, val.im);  printDelim();
}

void printLongInt(long int i) {
    transmitLongIntUART(downlinkbuffer, i);
}

/*void printInt_wo_delim(int i) {
    transmitIntUART_wo_delim(downlinkbuffer, i);
}*/

void printChar(char c) {
    transmitCharUART(downlinkbuffer, c);
}

void printFloat(float f) {
    transmitFloatUART(downlinkbuffer, f);
}

void printDouble(double f) {
    transmitDoubleUART(downlinkbuffer, f);
}

void printEL(void) {
    transmitEndLineUART(comdelay);
}

void printDelim(void){
    writeDelimiterToUART();
}

void set_comdelay(int c) {
    comdelay = c; // is that common delay?
}

void transmit_last_recorded_depth(void) {
    
    double d = get_last_recorded_depth();
    if (d == (double) NO_ECHO) {
        if (get_Mode_autoping().state == TRUE) {
            printString("-, V="); printInt(get_voltage());
        } else {
            printString("No echo, V="); printInt(get_voltage());
        }
    } else {
        printDouble(d);
        printString(" m, V="); printInt(get_voltage());
        printString(" (min="); 
        printDouble(get_depth_min());
        printString(", max="); 
        printDouble(get_depth_max());
        printString(")");
    }
    printEL();

}

void transmit_storage_vector(void){
    print_start_mark("STORAGE_VECT");
    int32_t* p=get_storage_vector();
    int plen=get_storage_vector_len();
    int i;
    for(i=0;i<plen;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("STORAGE_VECT");
}

void transmit_storage_vector_N(int N){
    print_start_mark("STORAGE_VECT");
    int32_t* p=get_storage_vector();
    int i;
    for(i=0;i<N;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("STORAGE_VECT");
}

void transmit_storage_vector_test(void)
{
    print_start_mark("STORAGE_VECT_TEST");
    int32_t* p=get_storage_vector_test();
    int plen=get_storage_vector_test_len();
    int i;
    for(i=0;i<plen;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("STORAGE_VECT_TEST");
}

void transmit_storage_vector_test_N(int N)
{
    print_start_mark("STORAGE_VECT_TEST");
    int32_t* p=get_storage_vector_test();
    int i;
    for(i=0;i<N;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }
    print_end_mark("STORAGE_VECT_TEST");
}

//void transmit_storage_vector_dc_N(int N){ // used to transmit only N values not the whole vectors
//    int32_t* p=get_storage_vector_dc();
////    int plen=get_storage_vector_len();
//    int i;
//    for(i=0;i<N;i++){
//        printLongInt(p[i]); printDelim(); printEL();
//    }
//}

void transmit_correlation_values(void){
    int32_t* p=get_corr_save();
    int i;
    for(i=0;i<CORR_SAVE_LEN;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }   
}

void transmit_time_values(void){
    int32_t* p=get_time_save();
    int i;
    for(i=0;i<TIME_SAVE_LEN;i++){
        printLongInt(p[i]); printDelim(); printEL();
    }    
}

void transmit_sonar_settings(void){
    //int commdelay=100;
    // Print Sonar settings
    printString("Sonar ID: "); 
    printInt(SONAR_ID); printEL();
    delaymicro(comdelay);
    
    printString("Transducer center frequency: "); 
    printInt(get_frequency()); printEL();
    delaymicro(comdelay);
    
    printString("Pulse voltage: "); 
    printInt(get_voltage()); printEL();   
    delaymicro(comdelay);
    
    printString("Number of pulses: "); 
    printInt(get_np()); printEL();   
    delaymicro(comdelay);
    
    printString("Samling frequency: "); 
    printInt(get_Fs()); printEL();
    delaymicro(comdelay);
    
    printString("Start delay: "); 
    printInt(get_startdelay()); printString(" [us]"); printEL();
    delaymicro(comdelay);
   
    printString("ADC buffer size: ");
    printInt(get_adc_buffer_len()); printEL();
    delaymicro(comdelay);
    
    printString("Storage vector size: ");
    printInt(get_storage_vector_len()); printEL();
    delaymicro(comdelay);
    
    printString("Speed of sound for distance calculation: ");
    printInt(get_soundspeed()); printString(" m/s"); printEL();
    delaymicro(comdelay);
    
    printString("Last recorded depth: ");
    printDouble(get_last_recorded_depth());
    printString("(");printDouble(get_depth_min());
    printString("-");printDouble(get_depth_max()); 
    printString(") [m]");
    printEL();
    delaymicro(comdelay);
    
    printString("Last recorded depth correlation: ");
    printLongInt(get_last_recorded_depth_correlation()); 
    printEL();
    delaymicro(comdelay);
    
//    printString("Index in storage of echo: ");
//    printInt(get_trig_index());
//    printEL();
//    delaymicro(comdelay);
    
//    printString("Index in storage of first triggered: ");
//    printInt(get_trig_index_min());
//    printEL();
//    delaymicro(comdelay);
    
//    printString("Index in storage of last triggered: ");
//    printInt(get_trig_index_max());
//    printEL();
//    delaymicro(comdelay);
    
//    printString("Detector threshold: ");
//    printLongInt((long int)get_mf_detector_thr()); printEL();
//    delaymicro(comdelay);
    
//    printString("Nominal threshold scaler: ");
//    printInt(get_thrscale_nom()); printEL();
//    delaymicro(comdelay);

//    printString("Reverb threshold scaler: ");
//    printInt(get_thrscale_reverb()); printEL();
//    delaymicro(comdelay);
    
//    printString("Minimum threshold: "); 
//    printLongInt(get_minthresh()); printEL();
//    delaymicro(comdelay);
    
    // Detector performance indicators
    printString("Time to process one frame: ");
    printDouble(1000*get_detector_extime()); 
    printString(" [ms]"); printEL();
    delaymicro(comdelay);
    
    printString("Time of one frame: ");
    printDouble(1000*get_t_frame()); 
    printString(" [ms]"); printEL();
    delaymicro(comdelay);
    
    printString("Detector deadtime: ");
    printDouble(1000*get_deadtime());
    printString(" [ms]"); printEL();
    delaymicro(comdelay);
    
    printString("Detector timeout: ");
    printDouble(get_timeout());
    printEL();
   
}

int get_comdelay(void){
    return comdelay;
}

/*
// Transmits some sonar parameters. ( NEEDS TO BE REWORKED )
void transmitExtractOfAllVariables(char *UARToutbuffer, int comdelay) {
    printString("Variables incoming!"); printEL();       
    //transmitStringUART(UARToutbuffer, "version: ");
    //transmitIntUART(UARToutbuffer, version);
    //transmitEndLineUART(comdelay);
}
 * */