/* 
 * File:   default_values.h
 * Author: Viktor
 *
 * Created on den 2 augusti 2017, 13:55
 */

#ifndef DEFAULT_VALUES_H
#define	DEFAULT_VALUES_H
#include <stdint.h>     // Necessary for int16_t declaration
#ifdef	__cplusplus
extern "C" {
#endif
    
// DEVICE SETUP
#define DEVICE_BEACON 0
#define DEVICE_TRANSMITTER 1

//int current_device = DEVICE_BEACON;
int current_device = DEVICE_TRANSMITTER;
    

#define NOISE_LISTENING_TIME 5 // 5sec before the detector is on

/* User settings on the PIC32 */
//#define FCY 150000000       // Timer clock speed, in Hz
//#define FPB 150000000
#define FCY 126000000       // Timer clock speed, in Hz
#define FPB 126000000       //  Peripheral clock speed, in Hz
#define BAUDRATE1 921600    // Baudrate on main serial port, in bits/s 
#define BAUDRATE5 9600      // Baudrate for comm with GPS module
#define BRGVAL ((FCY/BAUDRATE1)/4)-1 // default value in pic 32
    
// Definitions of constants used in the code, to 
// improve readability.
#define TRUE 1
#define FALSE 0
#define DELIM ';'
#define NO_ECHO -1
    
#define LISTEN_MODE 0
#define PULSE_MODE 1
    
// ------------ General settings --------------------------------------    
#define DEFAULT_PULSENUM 10 // pulse number 
#define MAX_VOLTAGE 251 // System reboots for higher voltages when using 47kHz transducer
#define MIN_VOLTAGE 12 // There is a weird disturbance for lower voltages ()ebcause 50hz ac?)
#define MAX_FREQ_AUTOPING 0.5 // Can not go faster because of dead times / system not fast enough
// Default threshold for amplitude detector
#define DEFAULT_AMP_DETECTOR_THR 300
// Default interface mode
#define DEFAULT_HUMAN_INTERFACE TRUE
//#define DEFAULT_HUMAN_INTERFACE FALSE
// Commands received of the form '$SONAR_ID$xxx$'
#define SONAR_ID 0  
// Size of heap memory (bytes), set in compile options
#define HEAP_MEMORY_SIZE 1000
// Default length of the storage vector in memory.c
#define DEFAULT_STORAGE_LEN 20000
#define DEFAULT_COMDELAY 500 
// Default pulse voltage 
#define DEFAULT_VOLTAGE 100
// Default ADC sampling frequency
//#define DEFAULT_SAMPLING_FREQUENCY 160000
#define DEFAULT_SAMPLING_FREQUENCY 120000

#define MIN_PULSE_DELAY 0   
//#define MIN_PULSE_DELAY 750   // delay after pulsing, wait for 750 microseconds;  

// Running with 100kHz transducer ( DO NOT REMOVE ) ¤¤¤¤¤¤¤¤¤¤¤¤
//#define DEFAULT_RESONANCE_FREQUENCY 100000
//#define MIN_PULSE_DELAY 110 //[us]      (For the 100kHz transducer)
//#define DEFAULT_STARTDELAY 750

// Running with 47kHz transducer( DO NOT REMOVE ) ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤    
#define DEFAULT_RESONANCE_FREQUENCY 40000 
    
// -------------- Settings in water -------------------------------  
// Settings for using in water:    
#define DEFAULT_SOUNDSPEED 1500 // In water  
//#define DEFAULT_SOUNDSPEED 1460 // In water  

/*#define DEFAULT_MAX_RANGE 400 // [m] (controls timeout after a pulse)  
// Ringing is 7.4e5 in correlation at this time
// - min depth 65cm
#define DEFAULT_STARTDELAY 2000 // [us] ~0.6m    */

//#define DEFAULT_MF_DETECTOR_THR 2000000

// -------------- Settings in air ----------------
// Settings for using in air:    
//#define DEFAULT_SOUNDSPEED 343 // In air
#define DEFAULT_MAX_RANGE 50 // [m] (controls timeout after a pulse)  
#define DEFAULT_STARTDELAY 0 // [us] ~0.5m  
//#define DEFAULT_STARTDELAY 2000 // [us] ~0.5m  
#define DEFAULT_FIXED_THRESHOLD 2380 // default fixed threshold
#define DEFAULT_FIXED_THRESHOLD_DC 10000 // default fixed threshold
int fixed_threshold = DEFAULT_FIXED_THRESHOLD;
int fixed_threshold_dc = DEFAULT_FIXED_THRESHOLD_DC;

#define SENSITIVITY_SONOTRONICS_OMNI -185 // dB rel 1 uPa
#define AMP_GAIN 200            // Multiplication gain of input amplifier
#define VREF 3.661              // Reference voltage in adc (voltage of highest value)
#define V2ADC 1118.8            // ADCMAX/VREF
#define ADC_RESOLUTION 4096     // Maximum level in adc (resolution of the ADC) 
#define SONOTRONICS_FREQOFFSET 55 // Measured offset on sonotronics pinger

#define version 20200523   // Date of current version

// Set length of vectors that are related to dsp operations   
//#define LOG2NFFT 12        //log2 base of NFFT (log2(NFFT))
//#define NFFT 4096           // Length of DSP vectors (should be 2^N)	  

#define LOG2NFFT 11        //log2 base of NFFT (log2(NFFT))
#define LOG2NFFT_DC 6        //log2 base of NFFT_DC (log2(NFFT))
#define NFFT 2048           // Length of DSP vectors (should be 2^N)	  
#define NFFT_DC 64          // length of downconverted NFFT DC length
      
//#define MAX_SCALE 2097152      // (1023*1023) maximum value used to make sure no overflow happened
#define MAX_SCALE 1046529      // (1023*1023) maximum value used to make sure no overflow happened

//#define LOG2NFFT 10        //log2 base of NFFT (log2(NFFT))
//#define NFFT 1024           // Length of DSP vectors (should be 2^N)	  
    
//#define LOG2NFFT 9        //log2 base of NFFT (log2(NFFT))
//#define NFFT 512           // Length of DSP vectors (should be 2^N)	  
    
//#define LOG2NFFT 11        //log2 base of NFFT (log2(NFFT))
    
#define INT_EXPONENT 12     // For the fixed point implementation

#ifdef	__cplusplus
}
#endif

#endif	/* DEFAULT_VALUES_H */

