#include "messages.h"
#include "downlink.h"
#include "modes.h"
#include "memory.h"
#include "sonar.h"
#include "pulse.h"
#include "detection.h"
#include "delay.h"
#include "sampling.h"
#include "SONARconfig.h"

void printMessage(int msgcode){
    
    // Just transmit code if not using a human interface
    if (get_Mode_human_interface().state==FALSE){
        printString("$");
        printInt(msgcode); 
        printString("$");
        
        switch(msgcode){
            case MSG_GPS_POS:
                // Print Number
                break;
            case MSG_GPS_TIME:
                // Add number here
                break;
            case MSG_DEPTH: 
                if (get_last_recorded_depth() < 0) {
                    printDouble(-1); 
                    printString("$"); 
                    printDouble(-1); 
                    printString("$"); 
                    printDouble(-1); 
                   /* printInt(-1); 
                    printString("$"); 
                    printInt(-1); 
                    printString("$"); 
                    printInt(-1); */
                    /*printString("-");
                     printEL();*/
                 } else {
                    printDouble(get_depth_min()); 
                    printString("$"); 
                    printDouble(get_last_recorded_depth()); 
                    printString("$"); 
                    printDouble(get_depth_max()); 
                    /*printInt((int)(100)); 
                    printString("$"); 
                    printInt((int)(100)); 
                    printString("$"); 
                    printInt((int)(100));*/
                 }
                break;
            case MSG_DETECTION:
                printInt(1);
                break;
            case MSG_SIGENERGY:
                // Print value here
                break;
        }
        
        printString("$");
        printEL();
    }
    // Print text message
    else{
        
        switch(msgcode){    
            
            case MSG_TIMEOUT_DETECTION:
                printString("Detector time-out");
                printEL();
                break;
            case MSG_TIMEOUT_SAMPLING:
                printString("Detector sampling time-out (got stuck waiting for samples)");
                printEL();
                break;                    
            case MSG_STARTUP: 
                printString("--> Hello, I am ready, good luck :-> <--");
                printEL();
                break;
            case MSG_STORAGE_ALLOCATION_SUCCESSFUL:
                printString("Storage memory successfully allocated (size: ");
                printInt(get_storage_vector_len());
                printString(")");
                printEL();
                break;
            case MSG_BUFFER_ALLOCATION_SUCCESSFUL:
                printString("ADC buffer successfully allocated (size: ");
                printInt(get_adc_buffer_len());
                printString(" )");
                printEL();
                break;
            case MSG_ALLOCATION_UNSUCCESSFUL:
                printString("Error in allocating memory for storage "
                            ". Did you put enough memory into the "
                            "heap? (dynamic allocation uses more memory "
                            "than theoretical vector size) ");
                printEL();
                break;
            case MSG_TIMEOUT_RECEIVE_FILE:
                printString("No file received before timeout.");
                printEL();
                break;
            case MSG_WAITING_FOR_FILE:
                printString("Waiting for file with data values");
                printEL();
                break;
            case MSG_STORAGE_OVERFLOW:
                printString("Number of values sent larger than storage array "
                        "(size: ");
                printInt(get_storage_vector_len());
                printString("), some were overwritten.");
                printEL();
                break;
            case MSG_CMND_SV:
                printString("Voltage set to: ");
                printInt(get_voltage());
                printEL();
                break;
            case MSG_CMND_SF:
                printString("Frequency set to: ");
                printInt(get_frequency());
                printEL();
                break;
            case MSG_CMND_SNP:
                printString("Number of pulses set to: ");
                printInt(get_np());
                printEL();
                break;
            case MSG_CMND_AP:
                if (get_Mode_autoping().state == 0) {
                    printString("Auto-pinging disabled");
                } else if (get_Mode_autoping().state == 1) {
                    printString("Auto-pinging enabled");
                }
                printEL();
                break;
            case MSG_CMND_DBR:
                if (get_Mode_debug_r().state == 0) {
                    printString("Debug Receiving mode disabled, everything is cleared !");
                } else if (get_Mode_debug_r().state == 1) {
                    printString("Debug Receiving mode enabled, listening and detecting now !");
                }
                printEL();
                break;
            case MSG_CMND_SW:
                if (get_Mode_switcher().state == 0) {
                    printString("Switched successfully. Now it is LISTENING mode !");
                } else if (get_Mode_switcher().state == 1) {
                    printString("Switched successfully. Now it is PULSING mode !");
                }
                printEL();
                break;
            case MSG_CMND_DM:
                if (get_Mode_detector().state == TRUE) {
                    printString("Transducer is listening for pulses");
                } else if (get_Mode_detector().state == FALSE) {
                    printString("Transducer is listening for echoes");
                }
                printEL();
                break;
            case MSG_CMND_STR:
                printString("Fixed threshold is set to: ");
                printInt(get_fixed_threshold());                
                printEL();
                break;
            case MSG_CMND_SSF:
                printString("Scale factor is set to: ");
                printInt(get_scale_factor());                
                printEL();
                break;
            case MSG_CMND_SSD:
                printString("start delay is set to: ");
                printInt(get_startdelay());                
                printEL();
                break;
            case MSG_CMND_SQP:
                printString("Sending square pulse"); 
                printEL();
                break;
            case MSG_CMND_CHP:
                printString("Sending chirp pulse"); 
                printEL();
                break;
            case MSG_CMND_SP:
                printString("Speed of sound set to ");
                printInt(get_soundspeed());
                printEL();
                break;
            case MSG_CMND_PL:
                printString("Sending pulse and listening only");
                printEL();
                break;
            case MSG_CMND_THR_NOM:
                printString("Nominal threshold scaler set to: ");
//                printInt(get_thrscale_nom());
                printEL();
                break;
            case MSG_CMND_DW:
                printString("Detect waveform mode: "); printInt(get_Mode_detect_waveform().state);
                printEL();
                break;
            case MSG_GPS_POS:
                printString("GPS position: ");  // Add number here
                printEL();
                break;
            case MSG_GPS_TIME:
                printString("GPS time: ");  // Add number here
                printEL();
                break;
            case MSG_DETECTION:
                printString("Signal detected");
                printEL();
                break;
            case MSG_DEPTH:
                if (get_last_recorded_depth() < 0) {
                    printString("$"); 
                    printDouble(-1);
                    printString("$"); 
                    printDouble(-1);
                    printString("$"); 
                    printDouble(-1);
                    printString("$");
                    printEL();
                    
                    /*printString("-");
                     printEL();*/
                 } else {
                    printString("$"); 
                    printDouble(get_depth_min());
                    printString("$"); 
                    printDouble(get_last_recorded_depth());
                    printString("$"); 
                    printDouble(get_depth_max());
                    printString("$"); 

                     /*printDouble(get_last_recorded_depth());
                     printString(" (");
                     printDouble(get_depth_min());
                     printString("-");
                     printDouble(get_depth_max());
                     printString(")");
                     printEL();*/
                    printEL();
                 }
                break;
            case MSG_SIGENERGY:
                // Add functionality here
                break;
            case MSG_CMND_THR_REVERB:
                printString("Reverb threshold scaler set to: ");
//                printInt(get_thrscale_reverb());
                printEL();
                break;
            case MSG_CMND_PLC: 
                printString("Pinging and listening/correlating with waveform");
                printEL();
                break;
 
            default:
                return;
        }
    
    }
    
}
















