#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

#include "math.h"
#include "downlink.h"
#include "pulse.h"
#include "SONARconfig.h"
#include "delay.h"
#include "sonar.h" 
#include "detection.h"
#include <string.h>

int32_t t_pulsestart = 0;

// Assembler functions
extern void setLATD9(void);
extern void clearLATD9(void);
extern void setLATD10(void);
extern void clearLATD10(void);

// Set LATD
int LATD9 = (1 << 9);
int LATD10 = (1 << 10);
int LATD0 = (1 << 0);
int LATD11 = (1 << 11);

volatile int startdelay = DEFAULT_STARTDELAY;
volatile int mb = 0; //mb: counter for the PWM


/* pulses.c ************************************************
 * This file contains functions that generates different type
 * of pulses for the sonar, and functions that sets the voltage 
 * level for the transducer. 
 */


/****************************************************************
 * Timer used for creating square pulses, by inverting the voltage 
 * to the transducer. 
 */
void __attribute__((interrupt(IPL2AUTO), vector(_TIMER_1_VECTOR)))
Timer1Handler(void);


void Timer1Handler(void) {

    IFS0bits.T1IF = 0;
    TMR1 = 0;
    mb++;
    // Make the output move between high, gnd, low
    // for a more smooth wave than a pure square
    if (LATDbits.LATD0 == 1 ) {
        LATD = MIDPLANE_MINUS;
        pulse_pause();
        LATD = VPLUS;
    } else {
        LATD = MIDPLANE_PLUS;
        pulse_pause();
        LATD = VMINUS;
    }

}

/****************************************************************
 * Send a train of square pulses on the transducer.
 */
void send_square_pulse(volatile int periods, int frequency, int voltage) {
    //printString("sending square np="); printInt(periods); printEL();
    
    T4CONbits.TON = 0; // time interrupt, which is used to determine the period of occurrence of a certain action
    T1CONbits.TON = 0;
    // Put safety limit on voltage
    if (voltage > MAX_VOLTAGE) {
        voltage = MAX_VOLTAGE;
    }
    // Put safety limit on frequency
    if (frequency > 500000) {
        frequency = 500000;
    }
    // Put safety limit on number of pulses
    if (periods > 1000) {
        periods = 1000;
    }
    int32_t t0;
 //   // Make the transducer float while charging voltage
 //   LATDbits.LATD11 = 0; //1LoIn
 //   LATDbits.LATD9 = 0; //2LoIn
 //   LATDbits.LATD0 = 0; //1HiIn
 //   LATDbits.LATD10 = 0; //2HiIn
    
    //Activate power stage (must be called before HVsupply)
//    printString("hello!");
//    Time t1, t2;
//    t1.clk = get_time_clk();
    
    set_HVsupply(voltage); //max 640V p-p!!! (peak to peak voltage, rms))
    
//#if 0
//    t2.clk = get_time_clk();
//    printString("the time for charging the capacitor is "); printDouble(get_time_diff(t1, t2)); printEL();
    setTransducerToPulsing();

    set_frequency(frequency);
    mb = 0; //zero the counter

    //TMR4 = 0; //Zero 32-bit timer
    TMR1 = 0; //Zero 16-bit timer
    T4CONbits.TON = 1; //activate 32-bit 150MHz timer
    //int t0 = TMR4; // Pulse start time  
    
  //  // Set first output square (stop floating output)
  //  LATD = MIDPLANE_MINUS;
  //  pulse_pause();
  //  LATD = VPLUS; 
    //Activate timer for pulsetrain, interrupts are used

    T1CONbits.TON = 1; // time 1 interruption is on

    t0=TMR4;
    while (mb <= (periods + 3)*2); //mb increased when interrupt timer is fired until number of pulses is reached
    
    T1CONbits.ON = 0; //stop pulse out timer
    //flip=0;

    // is the (pulses + 3)*2 a hack from cadson?
    //int t0 = TMR4;

    //set_t_pulsestart(t0); // Save time when pulsing part was finished
    //set_t_pulsestart(TMR4); // Save time when pulsing part was finished

    mb = 0; 
    //#endif

    // Put GND to +/- of transducer, to bleed off the ringing
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD10 = 0; //2HiIn

    // Wait while the oscillations on the transducer are being attenuated, 
    // needs a certain time depending on number of pulses/voltage
//    printString("start delay is "); printInt(startdelay); printEL();
//    if (startdelay >= MIN_PULSE_DELAY) {
////        delaymicro_calibrated(startdelay);
//        delaymicrosec(startdelay);
//    } else {
////        delaymicro_calibrated(MIN_PULSE_DELAY);
//        delaymicrosec(MIN_PULSE_DELAY);
//    }

    set_t_pulsestart(t0); // Save time when pulsing part was started

    // Make the transducer float, before activating input amplifier
    LATDbits.LATD11 = 0; //1LoIn
    LATDbits.LATD9 = 0; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

// This additional delay is found by experimenting, which tells
// that the transducers cannot immidiatedly start_samplig right after
// pulsing back, so one more delay is added to achieve the functionality
//    if (startdelay >= MIN_PULSE_DELAY) {
//        delaymicro(startdelay);
//    } else {
//        delaymicro(MIN_PULSE_DELAY);
//    }
    
    //printLongInt(t2.clk - t1.clk); printDelim(); printEL();

}

void send_chirp_pulse(int periods, int voltage, int frequency, int bandwidth) {
    //printString("sending chirp np="); printInt(periods); printEL();

    int f1 = frequency - (bandwidth / 2);
    int f2 = frequency + (bandwidth / 2);
    
    // Pulse time
    //double T=periods/(double)frequency;

    // Put safety limit on voltage
    if (voltage > MAX_VOLTAGE) {
        voltage = MAX_VOLTAGE;
    }
    // Put safety limit on frequency
    if (f2 > 500000) {
        f2 = 500000;
    }
    // Put safety limit on number of pulses
    if (periods > 1000) {
        periods = 1000;
    }

    set_frequency(f1);
    set_HVsupply(voltage);

    mb = 0; //zero the counter
    int PRRR = 0; //Pulse period range var
    setTransducerToPulsing();

    TMR4 = 0; //Zero 32-bit timer
    TMR1 = 0; //Zero 16-bit timer  

    T4CONbits.TON = 1; //activate 32-bit 150MHz timer        
    int df=f2-f1;
    T1CONbits.TON = 1; //Activate timer for pulsetrain, interrupts are used
    
    int32_t t0 = TMR4; // Pulse start time
    do { //frequency vary
        if (mb != 0) {
            PRRR = f1 + ( (df / (2*periods)) * mb);
        } else {
            PRRR = f1;
        }
        set_frequency(PRRR);
    } while (mb < 2*(periods+ 3)); //mb increased when interrupt timer is fired until number of pulses is reached
    T1CONbits.TON = 0;
    //int t0 = TMR4; 
   
    //int t1=TMR4;
    
    // Put GND to +/- of transducer, to bleed off the ringing
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn
    
    // Wait while the oscillations on the transducer are being attenuated, 
    // needs a certain time depending on number of pulses/voltage
    startdelay=0;
    if (startdelay > MIN_PULSE_DELAY) {
        delaymicro(startdelay);
    } else {
        delaymicro(MIN_PULSE_DELAY);
    }
    
    set_t_pulsestart(t0); // Save time when pulsing part was finished
    // Reset frequency, otherwise get_frequency() might return f2, which is 
    // then set as f1
    set_frequency(frequency); 

    // Make the transducer float, before activating input amplifier
    LATDbits.LATD11 = 0; //1LoIn
    LATDbits.LATD9 = 0; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn
    
    //printString("Pulse time: "); printDouble(1000*((double)(t1-t0))/FCY ); printString(" ms"); printEL();

}

void send_combined_sequence(int pulses, int voltage, int frequency, int bandwidth) {

    // -------------------------
    pulses = 20; // FOR TESTING 
    // -------------------------

    // ---------------------------------------------------------
    // Set up and send chirp    
    // ---------------------------------------------------------

    int fromfreq = frequency - (bandwidth / 2);
    int tofreq = frequency + (bandwidth / 2);

    // Put safety limit on voltage
    if (voltage > MAX_VOLTAGE) {
        voltage = MAX_VOLTAGE;
    }
    // Put safety limit on frequency
    if (tofreq > 500000) {
        tofreq = 500000;
    }
    // Put safety limit on number of pulses
    if (pulses > 1000) {
        pulses = 1000;
    }

    set_frequency(fromfreq);
    set_HVsupply(voltage);

    mb = 0; //zero the counter
    int PRRR = 0; //Pulse period range var
    setTransducerToPulsing();

    TMR4 = 0; //Zero 32-bit timer
    TMR1 = 0; //Zero 16-bit timer  
    T1CONbits.TON = 1; //Activate timer for pulsetrain, interrupts are used
    T4CONbits.TON = 1; //activate 32-bit 150MHz timer                                //activate 16-bit 150MHz timer  
    do { //frequency vary
        if (mb != 0) {
            PRRR = fromfreq + (((tofreq - fromfreq) / pulses) * mb);
        } else {
            PRRR = fromfreq;
        }
        set_frequency(PRRR);
    } while (mb < pulses); //mb increased when interrupt timer is fired until number of pulses is reached
    T1CONbits.TON = 0;

    // Put GND to +/- of transducer, to bleed off the ringing
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

    // End chirp ---------


    delaymicro(300); // This is the delay between chirp and sinusoid

    // ---------------------------------------------------------
    // Send narrow band pulse    
    // ---------------------------------------------------------

    // -------------------------
    pulses = 10; // FOR TESTING 
    // -------------------------

    set_frequency(frequency);
    //TMR4 = 0; //Zero 32-bit timer
    TMR1 = 0; //Zero 16-bit timer
    T4CONbits.TON = 1; //activate 32-bit 150MHz timer
    int t0 = TMR4; // Pulse start time
    T1CONbits.TON = 1; //Activate timer for pulsetrain, interrupts are used
    while (mb <= (pulses + 3)*2); //mb increased when interrupt timer is fired until number of pulses is reached


    T1CONbits.ON = 0; //stop pulse out timer
    mb = 0;

    // Put GND to +/- of transducer, to bleed off the ringing
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD10 = 0; //2HiIn

    // Wait while the oscillations on the transducer are being attenuated, 
    // needs a certain time depending on number of pulses/voltage
    if (startdelay >= MIN_PULSE_DELAY) {
        delaymicro(startdelay);
    } else {
        delaymicro(MIN_PULSE_DELAY);
    }

    // Make the transducer float, before activating input amplifier
    LATDbits.LATD11 = 0; //1LoIn
    LATDbits.LATD9 = 0; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

    set_t_pulsestart(t0); // Save time when pulsing part was finished

}

//SPWM test run ONLY for 47kHz frequency setting unused;
void send_sinetrain(int numberofpulses, int frequency, int voltage) {
    set_HVsupply(voltage); //max 640V p-p!!!
    set_frequency(frequency);

    setTransducerToPulsing(); //Activate power stage
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

    unsigned char pwmv[64]; //storage for calculated positive sine vectors
    unsigned char pwmvvn[64]; //storage for calculated negative sine vectors
    unsigned int resolution = 158; //pwm resolution

    //int numofvals = 18; //number of values for sine about 47kHz
    int numofvals = 18; //number of values for sine about 47kHz
    //Build prototype for converting frequency to numofvals here
    //
    //**********************************************************

    int i = 0;
    memset(pwmv, 0, sizeof (pwmv));
    memset(pwmvvn, 0, sizeof (pwmvvn));
    for (i = 0; i < numofvals; i++) {
        pwmv[i] = 0.5 * resolution + 0.5 * resolution * sin(2 * 3.14159265359 * i / (numofvals));
        pwmvvn[i] = resolution - pwmv[i];
    }
    int ii = 0, ij = 0;
    int t0 = TMR4;
    T6CONbits.TON = 1;
    do {
        do {
            LATD = 513;
            while (TMR6 < pwmv[ij]);
            TMR6 = 0;
            LATD = 2560;
            while (TMR6 < pwmvvn[ij]);
            TMR6 = 0;
            ij++;
        } while (ij < numofvals);
        ij = 0;
        ii++;
    } while (ii < numberofpulses);
    LATDbits.LATD11 = 1; //1LoIn
    LATDbits.LATD9 = 1; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

    set_t_pulsestart(t0); // Save time when pulsing part was finished

    // Wait while the oscillations on the transducer are being attenuated, 
    // needs a certain time depending on number of pulses/voltage
    if (startdelay > MIN_PULSE_DELAY) {
        delaymicro(startdelay);
    } else {
        delaymicro(MIN_PULSE_DELAY);
    }

    delaymilli(8);
    LATDbits.LATD11 = 0; //1LoIn
    LATDbits.LATD9 = 0; //2LoIn
    LATDbits.LATD0 = 0; //1HiIn
    LATDbits.LATD10 = 0; //2HiIn

}


// Sets the voltage level on high voltage supply, which defines how 
// much power is put into the pulse on the transducer.
void set_HVsupply(int v_set) {

    if (v_set > MAX_VOLTAGE) {
        v_set = MAX_VOLTAGE;
    }

    int volt = v_set;
    TRISEbits.TRISE7 = 1;
    LATEbits.LATE7 = 1;
    ANSELEbits.ANSE7 = 1;
    double resultet = 0.0;
    double resultkeeper = 0.0;
    long count = 0;
    ADCCON1bits.ON = 0;
    ADCCSS1bits.CSS15 = 1; // AN15 (Class 3) set for scan
    ADCCSS1bits.CSS18 = 0; // AN18 (Class 3) remove from scan
    delaymilli(10);
    ADCCON1bits.ON = 1;
    int samples = 10;
    ADCCON3bits.GSWTRG = 0;

    
    do {
        delaymillimicron(100);
        ADCCON3bits.GSWTRG = 1;
        while (ADCDSTAT1bits.ARDY15 == 0);
        resultet += ADCDATA15;
        count++;
    } while (count < samples);
    resultet = resultet / samples;
    resultkeeper = (85 * (resultet / 4096 * 3.3) - 0.0299) - 10;
    //resultkeeper=(85*(resultet/4096*3.3)-0.0299);
    //printString("resultkeeper before charging: ");printDouble(resultkeeper); printEL();
    //printDouble(resultkeeper);
    //printEL();

    if (volt > resultkeeper) {
        Nop();
        Nop();
        resultet = 0;
        do {
            resultet = 0;
            count++;
            int voltinc = 0;
            do {
                LATDbits.LATD4 = 1;
                delaymillimicron(400);
                LATDbits.LATD4 = 0;
                delaymillimicron(200);
                voltinc++;
            } while (voltinc < 30);
            delaymillimicron(10000);
            int ycount = 0;
            do {
                delaymillimicron(100);
                ADCCON3bits.GSWTRG = 1;
                while (ADCDSTAT1bits.ARDY15 == 0);
                resultet += ADCDATA15;
                ycount++;
            } while (ycount < samples);
            resultet = resultet / samples;
            resultkeeper = (85 * (resultet / 4096 * 3.3) - 0.0299) - 10;
            //resultkeeper=(85*(resultet/4096*3.3)-0.0299);

            if (resultkeeper < 0) {
                resultkeeper = 0;
            }

        } while (resultkeeper < volt && count < 100000);
        
    }


    if (volt < resultkeeper + 5) {
        LATFbits.LATF5 = 0;
        resultet = 0;
        int ycount = 0;
        do {
            resultet = 0;
            ycount++;
            delaymillimicron(100000);
            count = 0;
            do {
                delaymillimicron(100);
                ADCCON3bits.GSWTRG = 1;
                while (ADCDSTAT1bits.ARDY15 == 0);
                resultet += ADCDATA15;
                count++;
            } while (count < samples);
            resultet = resultet / samples;
            resultkeeper = (85 * (resultet / 4096 * 3.3) - 0.0299) - 10;
            //resultkeeper=(85*(resultet/4096*3.3)-0.0299);

        } while (resultkeeper > volt && ycount < 20000);
       
        LATFbits.LATF5 = 1;
        Nop();
        Nop();
        
        //printString("ycnt "); printInt(ycount); printEL();
    }
    
    ADCCON1bits.ON = 0;
    //         TRISEbits.TRISE7=0;
    //         LATEbits.LATE7=0;
    ANSELEbits.ANSE7 = 0;
    /*printString("Result keeper after charging: ");
    printDouble(resultkeeper);
    printEL();
    printString("volt: ");
    printInt(volt);
    printEL();*/

}
 

// Switch some transistors on the board so that the transducer is connected to 
// the output H-bridge with high voltage supply (for creating square pulses), 
// instead of the input amplifier.
void setTransducerToPulsing(void) {

    LATEbits.LATE1 = 1; //Enable 1
    LATEbits.LATE2 = 1; //Enable 2
    LATBbits.LATB13 = 1;
    LATBbits.LATB14 = 1;  
    // Set to some config
    LATDbits.LATD0 = 1;
    LATDbits.LATD9 = 1;
    LATDbits.LATD11 = 0;
    LATDbits.LATD10 = 0;
    
    ANSELEbits.ANSE4 = 0;
    TRISEbits.TRISE4 = 0;
    LATEbits.LATE4 = 0;
    
}


// PLUSHIGH

void switchPLUS_HIGH(void) {
    LATDbits.LATD9 = 0;
    LATDbits.LATD10 = 1;
}
// PLUSLOW

void switchPLUS_LOW(void) {
    LATDbits.LATD10 = 0;
    LATDbits.LATD9 = 1;
}
// MINUSHIGH

void switchMINUS_HIGH(void) {
    LATDbits.LATD11 = 0;
    LATDbits.LATD0 = 1;
}
// MINUSLOW

void switchMINUS_LOW(void) {
    LATDbits.LATD0 = 0;
    LATDbits.LATD11 = 1;
}



// Getters and setters for pulse variables

void set_startdelay(int s) {
//    if (s > 501) {
//        startdelay = s;
//    } else {
////        startdelay = 0;
//        startdelay = 501;
//    }
    startdelay = s;
}

int get_startdelay(void) {
    return startdelay;
}

void set_t_pulsestart(int32_t tps) {
    t_pulsestart = tps;
}

int32_t get_t_pulsestart(void) {
    return t_pulsestart;
}

void pulse_pause(void) {
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
    Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
}

