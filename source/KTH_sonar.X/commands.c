#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include <stdio.h>
#include "pulse.h"
#include "sonar.h"
#include "detection.h"
#include "modes.h"
#include "flags.h"
#include "downlink.h"
#include "georeference.h"
#include "delay.h"
#include "memory.h"
#include "messages.h"
#include "SONARconfig.h"


////////////// DEBUG Yaolin
#include "sampling.h"

// yaolin debug test
unsigned int LOADNUM = 0;
//int number_of_values_loaded=0;

/*
 * Functions for processing commands that have been received. 
 */

// Load a previously saved echo from txt file
void cmnd_load_file(void){
    
    // Save file values to storage vector
    int data_bufflen = get_storage_vector_len();
    int32_t* data = get_storage_vector();
    
    //printString("Waiting for file with data values");printEL();
    printMessage(MSG_WAITING_FOR_FILE);
    char rx_bufflen=10;
    char rxbuffer[rx_bufflen]; // Round buffer for received characters
    char value[4]; // Maximum received value: 9999
    int i=0;
    int index=0;
    long int timeout=0;
    int CHAR2INT=48;
    
    long int TIMEOUT_USER=130000000; // Will wait for 8-10s
    // Timeout for characters. Critical values as some serial 
    // monitor programs can send values in blocks which causes a long
    // wait between two characters
    long int TIMEOUT_CHAR=13000000; 
    
    int MAX_DECIMALS=4; // Maximum number of decimals in the value
    unsigned int num_vals_loaded=0;
    
    // Clear buffers
    memset(rxbuffer, 0, sizeof (rxbuffer));
    memset(data, 0, sizeof (data_bufflen)); 
  
    // The user now has some time to start transfer of the data file, 
    // wait for a file until timeout
    while(U1STAbits.URXDA == 0 && timeout<TIMEOUT_USER){
        timeout++;
    }
    // Timeout has happened
    if(timeout==TIMEOUT_USER){
        printMessage(MSG_TIMEOUT_RECEIVE_FILE);
        //printString("No file received before timeout.");printEL();
    }
    // A file has started arriving
    else{
        int negative_value=0;
        do {
            timeout = 0;
            // Wait for incoming character
            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < TIMEOUT_CHAR);
            // Character has arrived, save it
            rxbuffer[i] = U1RXREG;
            
            // The decimal value that is arriving is negative
            if(rxbuffer[i]=='-' && negative_value==0){
                negative_value=1;
            }
            
            // Numbers are terminated by ';', so there is a 1-4 character number
            // (0-9999) in the buffer before the ';' character
            if(rxbuffer[i]==';'){
                int k=i; // Save index of the ';' character
                int j=0; // Index of number of decimal values that are saved   
                memset(value, 0, sizeof (value)); //clear string
                
                // In case ';' is situated at rxbuffer[0]
                if(k-j-1<0){
                    // Set k so that (bufflen+j)-j-1=bufflen-1
                    // in rxbuffer[k-j-1]
                    k=rx_bufflen+j; 
                }
                // Find all decimal value characters that arrived before the ';'
                // Look for a maximum of 4 decimal values (0<=j<=3)
                while( ('0'<=rxbuffer[k-j-1]&&rxbuffer[k-j-1]<='9') &&j<=MAX_DECIMALS-1 ){
                                        
                    // Store decimal value found in the receive buffer
                    value[MAX_DECIMALS-j-1]=rxbuffer[k-j-1]-CHAR2INT;                       
                    j++;
                            
                    // Looking for values at negative buffer index, look 
                    // at the end of the buffer
                    if(k-j-1<0){
                        // Set k so that (bufflen+j)-j-1=bufflen-1
                        // in rxbuffer[k-j-1]
                        k=rx_bufflen+j; 
                    }

                }
                // Calculate the decimal value from the positions relative to 
                // the ';' character of each decimal character, and save to 
                // data output vector                 
                data[index]=1000*value[0]+100*value[1]+10*value[2]+value[3];
                
                // Set correct sign
                if(negative_value==1){
                    data[index]=-data[index];
                    negative_value=0;
                }
                
                num_vals_loaded++; // Keep track of number of loaded values
                index++;           // Advance data vector index
                if(index>=data_bufflen){ 
                    // Go around data buffer if too long vector is sent
                    index=0;
                }
                
            }   
            // A decimal value has been processed
            
            i++;    // Advance receive buffer index 
            // Reached end of receive buffer, start from the beginning
            if(i>=rx_bufflen){
                i=0;
            }
            
        // Break if no character was found before timeout
        } while ( timeout < TIMEOUT_CHAR); 
        
        //number_of_values_loaded=num_vals_loaded;
        // Print how many values have been loaded
        if(get_Mode_human_interface().state==TRUE){
            printInt(num_vals_loaded);printString(" values loaded."); 
            printEL();
        }
        if(num_vals_loaded>data_bufflen){
            printMessage(MSG_STORAGE_OVERFLOW);
        }
    } 
    LOADNUM = num_vals_loaded;
//    No more values are arriving, stop listening
}

void cmnd_set_frequency(char *data) {
    char substr1[strlen(data) - 2];
    strncpy(substr1, &data[2], strlen(data) - 1);
    int freq = 0;
    sscanf(substr1, "%d", &freq);
    set_frequency(freq);
    printMessage(MSG_CMND_SF);
}

void cmnd_set_fixed_threshold(char *data) {
    char substr1[strlen(data) - 3];
    strncpy(substr1, &data[3], strlen(data) - 1);
    int threshold = 0;
    sscanf(substr1, "%d", &threshold);
    set_fixed_threshold(threshold);
    printMessage(MSG_CMND_STR);
}

void cmnd_set_scale_factor(char *data) {
    char substr1[strlen(data) - 3];
    strncpy(substr1, &data[3], strlen(data) - 1);
    int scale = 0;
    sscanf(substr1, "%d", &scale);
    set_scale_factor(scale);
    printMessage(MSG_CMND_SSF);
}

void cmnd_set_start_delay(char *data) {
    char substr1[strlen(data) - 3];
    strncpy(substr1, &data[3], strlen(data) - 1);
    int start_delay = 0;
    sscanf(substr1, "%d", &start_delay);
    print_val("start_delay", start_delay);
    set_startdelay(start_delay);
    printMessage(MSG_CMND_SSD);
}

void cmnd_set_number_of_pulses(char *data,int cmdlen) {
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int np_ = 0;
    sscanf(substr1, "%d", &np_);
    set_np(np_);
    printMessage(MSG_CMND_SNP);

}


void cmnd_set_voltage(char *data) {
    char substr1[strlen(data) - 2];
    strncpy(substr1, &data[2], strlen(data) - 1);
    int voltage = 0;
    sscanf(substr1, "%d", &voltage);
    set_voltage(voltage);
    printMessage(MSG_CMND_SV); 
    
}

void cmnd_set_storage_vector_len(char *data,int cmdlen) {
    /* DYNAMIC MEMORY STORAGE CURRENTLY REMOVED (FOR STABILITY) */
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int len = 0;
    sscanf(substr1, "%d", &len);
    
    // Always deallocate first, then change   
    // length and allocate
//    deallocate_storage_vector();
//    allocate_storage_vector(len);

}

void cmnd_set_soundspeed(char *data) {
    char substr1[strlen(data) - 2];
    strncpy(substr1, &data[2], strlen(data) - 1);
    int sp = 0;
    sscanf(substr1, "%d", &sp);
    set_soundspeed(sp);
    printMessage(MSG_CMND_SP);
}

void cmnd_set_apmode(void) {
    toggle_Mode_autoping();
    printMessage(MSG_CMND_AP);
}

void cmnd_set_debug_r_mode(void) {
    toggle_Mode_debug_r();
    printMessage(MSG_CMND_DBR);
}

void cmnd_set_switcher_mode(void) {
    toggle_Mode_switcher();
    printMessage(MSG_CMND_SW);
}

void cmnd_set_detector_mode(void) {
    toggle_Mode_detector();
    printMessage(MSG_CMND_DM);
}

void cmnd_detect_waveform(void){
    toggle_Mode_detect_waveform();
    printMessage(MSG_CMND_DW);
}

/*void cmnd_set_mf_detector_threshold(char *data, int cmdlen){
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    long int thr = 0;
    sscanf(substr1, "%ld", &thr);

    printString("MF detector threshold set to: ");
    printLongInt(thr);
    printEL();
    set_mf_detector_thr((int32_t)thr);
}*/

void cmnd_send_square_pulse(void) {
    printMessage(MSG_CMND_SQP);
    set_Flag_cmd_send_pulse(TRUE);
}

void cmnd_send_chirp_pulse(void) {
    printMessage(MSG_CMND_CHP);
    set_Flag_cmd_send_pulse(TRUE);
}

void cmnd_download_storage_vector(void){
    delaymicro(get_comdelay());
    transmit_storage_vector();
    delaymicro(get_comdelay());
}

void cmnd_download_correlation_vector(void){
    delaymicro(get_comdelay());
    transmit_correlation_values();
    delaymicro(get_comdelay());
}

void cmnd_download_time_vector(void){
    delaymicro(get_comdelay());
    transmit_time_values();
    delaymicro(get_comdelay());
}

void cmnd_print_sonar_settings(void) {
    transmit_sonar_settings();
}

void cmnd_send_pulse_listen(void) {
    set_Flag_cmd_send_pulse_listen(TRUE);
    printMessage(MSG_CMND_PL);
}

void cmnd_send_pulse_listen_correlate(void) {
    set_Flag_cmd_send_pulse_listenc(TRUE);
    printMessage(MSG_CMND_PLC);
}

void cmnd_debug_mode(void) {
    toggle_Mode_debug();
    printString("Debug mode status: ");
    printChar(get_Mode_debug().state);printEL(); 
}
void cmnd_debug(void) {
    set_Flag_cmd_debug(TRUE);
    printString("Executing debug command");
    printEL(); 
}

void cmnd_debug_r(void) {
    set_Flag_cmd_debug_r(TRUE);
    printString("Executing receiver debug command");
    printEL(); 
}

void cmnd_set_debug_var(char *data,int cmdlen) {
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int debug_var = 0;
    sscanf(substr1, "%d", &debug_var);

    // **** Use set_variable(debug_var) function here  ****

    printString("Debug variable set to: ");
    printInt(debug_var);
    printEL();
}

void cmnd_set_amp_detector_threshold(char *data, int cmdlen){
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int amp_detector_thr = 0;
    sscanf(substr1, "%d", &amp_detector_thr); 
//    set_amp_detector_thr((int16_t)amp_detector_thr);
    printString("Amplitude detector threshold set to: ");
    printInt(amp_detector_thr);
    printEL();
}

void cmnd_set_thrscale_nom(char *data,int cmdlen){
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int th = 0;
    sscanf(substr1, "%d", &th); 
//    set_thrscale_nom(th);
    printMessage(MSG_CMND_THR_NOM);

}

void cmnd_set_thrscale_reverb(char *data,int cmdlen){
    char substr1[strlen(data) - cmdlen];
    strncpy(substr1, &data[cmdlen], strlen(data) - 1);
    int th = 0;
    sscanf(substr1, "%d", &th); 
//    set_thrscale_reverb(th);
    printMessage(MSG_CMND_THR_REVERB);
}

void cmnd_set_comdelay(char *data) {
    char substr1[strlen(data) - 2];
    strncpy(substr1, &data[2], strlen(data) - 1);
    int comdelli = 0;
    sscanf(substr1, "%d", &comdelli);
    set_comdelay(comdelli);

    printString("UART delay set to ");
    printInt(comdelli);
    printString(" microseconds");
    printEL();
}

void cmnd_activate_GPS(void) {
    //LATBbits.LATB3 = 1;
    delaymilli(1000);
    set_GPSact(1);
    start_GPS();
    //activateTimer6();
    printString("GPS output activated");
    printEL();
}

void cmnd_deactivate_GPS(void) {
    //LATBbits.LATB3=0;
    set_GPSact(0);
    stop_GPS();
    //inactivateTimer6();
    printString("GPS output deactivated");
    printEL();
}

void cmnd_activate_GPS_feedthrough(void) {
    // Power up gps?
    LATBbits.LATB3 = 1;
    delaymilli(2000);  // Wait long enough for system to be stable
    set_GPSfeed(1);
    printString("GPS feedthrough activated");
    printEL();
}

void cmnd_deactivate_GPS_feedthrough(void) {
    // Power off gps?
    set_GPSfeed(0);
    LATBbits.LATB3 = 0;
    delaymilli(2000);   // Wait long enough for system to be stable
    printString("GPS feedthrough deactivated");
    printEL();
}

void cmnd_show_GPS_time(void) {
    if (get_GPSact() == 1) {
        printString("GPS time / date: ");
        printString(get_timestringSet());
        printString(":");

        printInt(TMR6 * 10 * get_timeMultiplicator());

        printString(" / ");
        printString(get_datestringSet());

    } else {
        printString("GPS inactive");
    }
    printEL();
}

void cmnd_show_GPS_fix(void) {
    if (get_GPSfix() > 1) {
        printString("GPS fix");
    } else {
        printString("GPS no fix");
    }
    printEL();
}

void cmnd_show_GPS_location(void) {
    if (get_GPSfix() > 1) {
        printString("GPS location: ");
        printString(get_locationstring());
    } else {
        printString("GPS no fix");
    }
    printEL();
}

void cmnd_activate_timecalibration(void) {
    if (get_GPSact() == 1 && get_GPSfix() > 1) {
        printString("Timer calibration started, wait..");

        volatile int vall = PPStoCrystalCal();
        if (vall < 999999999 && vall > 149000000 && vall < 151000000) {
            printString("Timer calibrated to ");
            unsigned long nss = vall;
            printInt(nss);
            printString("Hz");

        } else {
            printString("Error: Calibration failed, signal dropped during calibration");
        }
    } else {
        if (get_GPSact() == 0) {
            printString("Error: GPS not activated");
        } else {
            printString("Error: GPS no fix");
        }
    }
    printEL();
}

void cmnd_activate_cyclic_output(void){
    printString("Cyclic output activated");
    printEL();
    set_GPScyclic(1); 
}

void cmnd_deactivate_cyclic_output(void){
    printString("Cyclic output activated");
    printEL();
    set_GPScyclic(0); 
}

void cmnd_download_correlation_waveform(void){
    double* w=get_waveform();
    int i;
    for (i=0;i<NFFT;i++){
        printDouble(w[i]);printDelim();printEL();
    }
}

void cmnd_clear_everything(void){
    ini_everything();
    printString("BUFFER IS CLEARED SUCCESSFULLY!"); printEL();
}

void cmnd_stop_auto_ping(void){
    printString("AutoPing is stopped");
    printEL();
    set_Mode_autoping(FALSE);
}

unsigned int get_load_num(void)
{
    return LOADNUM;
}
