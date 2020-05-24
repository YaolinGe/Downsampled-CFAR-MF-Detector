#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include "delay.h"
#include "SONARconfig.h"
#include "setup.h"
#include "sonar.h"
#include "modes.h"
#include "flags.h"
#include "downlink.h"
#include "memory.h"
#include "messages.h"
#include "dsp.h"
#include "detection.h"
#include "sampling.h"

/****************************************************************
 * Call on all setup-functions, which are necessary to operate 
 * the sonar.
 */
void setup(void) {
    Nop();
    Nop();
    TRISB = 0xFFFF;
    TRISC = 0xFFFF;
    TRISD = 0xFFFF;
    TRISE = 0xFFFF;
    TRISF = 0xFFFF;
    TRISG = 0xFFFF;
    Nop();
    Nop();
    delaymilli(1000); 
    init_ports();    /// I guess what port needs to be initialised
    init_pps();   // PPS interrupt gets triggered by 50kHz transducer
    init_uarts(BAUDRATE1, FPB);
    init_interrupts();   //time interrupt to achieve a certain action at certain time
    delaymilli(1000);   /// why needs another time delay?
    init_modes();
    // Dynamic allocation removed (storage vector is allocated static)
    //allocate_storage_vector(get_storage_vector_len());
//    init_buffers();
    clear_flags();
    
    // Keep GPS off
    LATBbits.LATB3 = 0;

    // This function needs to be run to actually set the frequency
    set_frequency(DEFAULT_RESONANCE_FREQUENCY); 
    printMessage(MSG_STARTUP);
    printString("--> Underwater beacon ready for operation  <--");
    printEL();
    
    // For debugging
//    setTransducerToPulsing();
//    setTransducerToSensing(); 
    
    // Fill sampling buffer with values
    init_adc_buffer();
    
    // Initiate twiddle factors for dsp functions
    DSP_TransformFFT32_setup(get_twiddles32(), LOG2NFFT);
    DSP_TransformFFT32_setup(get_twiddles32_dc(), LOG2NFFT_DC);

// ================= BEACON SETUP ==============================================
    if(current_device == DEVICE_BEACON)
    {
        printString("beacon setup is done! good luck!"); printEL();
        printString("current device is "); printInt(current_device); printEL();
//        long int B = get_bandwidth();
//        long int B = 5000;
        set_bandwidth(2500);
    //    long int fc = get_frequency();
        set_frequency(40000);
        setup_downconversion(get_frequency(), 5000);
        setup_MatchedFilter(6.0,SINE);
        int Nframes = NOISE_LISTENING_TIME * get_Fs() / get_Nframe();
        ini_threshold_buffer_dc_corr(Nframes); // initialise the threshold buffer
    }
    
// =============== TRANSMITTER SETUP ===========================================
    else if(current_device == DEVICE_TRANSMITTER)
    {
        printString("transmitter setup is done! good luck!"); printEL();
        printString("current device is "); printInt(current_device); printEL();
//        long int B = get_bandwidth();
//        long int B = 5000;
        set_bandwidth(2500);
        set_frequency(40000);
    //    print_val("frequency", get_frequency());
        setup_downconversion(get_frequency(), 5000);
        setup_MatchedFilter(6.0,SINE);
        int Nframes = NOISE_LISTENING_TIME * get_Fs() / get_Nframe();
        ini_threshold_buffer_dc_corr(Nframes); // initialise the threshold buffer
//        ini_threshold_buffer_dc_corr_transmitter_side(Nframes); // initialise the threshold buffer
    }
    
// ================ END of DC SETUP ============================================
    
    
    // Some default (good) settings
    if(get_wave_select()==SINE){
        set_np(10);
    } else if(get_wave_select()==CHIRP){
        set_np(120); // Works well with a chirp around 40-50kHz
    }
    
    // Initiate buffer with bias values
  //  listen(2*NFFT,FALSE);   
    
}


// Initiate modes for default startup; going into idle mode and 
// wainting for a command. 
void init_modes(void){
    // Set possible idle mode (instead of sleep)
    
    set_Mode_autoping(FALSE);
    set_Mode_detect_waveform(FALSE);
    //set_Mode_rawoutput(FALSE);
    set_Mode_idle(TRUE);
    set_Mode_ramping_up(FALSE);
    set_Mode_ramping_down(FALSE);
    set_Mode_autoset_voltage(FALSE);
    set_Mode_debug(FALSE);
    set_Mode_debug_r(FALSE);
    set_Mode_switcher(FALSE);
    set_Mode_detector(FALSE);

    set_Mode_human_interface(DEFAULT_HUMAN_INTERFACE);
}
/****************************************************************
 * Below are functions that initiate specific functionalities
 * in the sonar (communication IO, transducer IO etc.).
 */
void init_interrupts(void) {

    asm volatile("di"); // This is the "KEY" to Disable "Off" All Interrupts
    
    // "Execution Hazard Barrier": Makes sure that the previous 
    // statement has been executed  before proceeding
    asm volatile("ehb"); 

    // System interrupt priority (2) lower than all others (sleep works?)
    //IPC0=2<<2;

    INTCONbits.MVEC = 1; // Enable Multi-vectored Interrupts
    PRISS = 0x76543210;
    IFS0bits.T1IF = 0;  // Clear the timer interrupt status flag
    IEC0bits.T1IE = 1;  // Enable interrupt for Timer1
    IPC1bits.T1IP = 1;  // Set priority (what priority does this set to?)
    
    // Set adc18 interrupt to zero
    IFS2bits.ADCD18IF = 0;  // Reset interrupt

    // Set up interrupts for timer 6/7 (GPS))
//    IFS1bits.T7IF = 0;  // Clear interrupt on Timer 6/7
//    IEC1bits.T7IE = 1;  // Enable interrupts on timer 6/7
//    IPC8bits.T7IP = 1;  // Interrupt priority 1 on Timer 6/7
    
    IFS2bits.ADCD10IF = 0;  // Disable adc interrupt flag, then enable 
    IEC2bits.ADCD10IE = 1;  // it to be triggered
    INTCONbits.INT4EP = 1;  //interrupt on rising edge
    
    IPC5bits.INT4IP = 1;    // External interrupt 4 priority
    IPC5bits.INT4IS = 1;    // External interrupt 4 sub-priority
    IFS0bits.INT4IF = 0;    // External interrupt 4 flag
    
//// !!!!!!!!!!!!!!!!!!!!!    
    // Disable this interrupt curerntly, gets triggered by transducer
    // needs to be fixed for GPS
    IEC0bits.INT4IE = 1;    // External interrupt 4 enable
// Works when not setting at all (default =0), but if actively
// set to zero,     IEC0bits.INT4IE = 0; , the program crashes on 
// autoping (not with simple pulsetrain command)

    // Disable GPS interrupt until it is needed
    TRISEbits.TRISE5=0;
    LATEbits.LATE5=0;
    
    IFS4bits.DMA0IF = 0;    // I2C2 master event flag
    IEC4bits.DMA0IE = 1;    // I2C2 master event enable
    asm volatile("ei");
    __builtin_enable_interrupts();
    
    return;
}

// Initiate UART configuration (serial line transimission setup))
void init_uarts(long int baudrate, long int fpb) {   
    
    int ON = 15;
    int PDSEL = 1;
    int STSEL = 0;
    int BRGH = 3;
    int UTXEN = 10;
    int URXEN = 12;

    U1BRG = FPB / (4.0*BAUDRATE1) - 1;
    U1MODE =
        (1 << ON) |       // UART1 enabled
        (1 << BRGH) |     // baudrate generation with equation 21-2
        (0b00 << PDSEL) | // 8 databits, no parity (default)
        (0 << STSEL);     // 1 stopbit (default)
    U1STA =
        (1 << UTXEN) |    // transmission enabled
        (1 << URXEN);     // reception enabled

    // GPS
    U5BRG = FPB / (4.0*BAUDRATE5) - 1;
    U5MODE =
        (1 << ON) |       // UART5 enabled
        (1 << BRGH) |     // baudrate generation with equation 21-2
        (0b00 << PDSEL) | // 8 databits, no parity (default)
        (0 << STSEL);     // 1 stopbit (default)
    U5STA =
        (1 << UTXEN) |    // transmission enabled
        (1 << URXEN);     // reception enabled
    
    // Enable wake up on uart interrupt
    //U1MODEbits.WAKE=1;
    //U5MODEbits.WAKE=1;
    
    // Enable interrupt on uart 1
    //IEC3bits.U1RXIE=1;
    // Enable interrupt on uart 5
    //IEC5bits.U5RXIE=1;
}

// Initiate ???
void init_ports(void) {
    TRISDbits.TRISD1 = 1;       //TRG

    TRISCbits.TRISC14 = 0;      //TRCD1 HIGH OUT
    TRISCbits.TRISC13 = 0;      //TRCD1 LOW OUT
    TRISDbits.TRISD0 = 0;       //TRCD upper bridge enable
    TRISDbits.TRISD2 = 1;       //UART receive
    TRISDbits.TRISD3 = 0;       //UART transmit
    TRISDbits.TRISD10 = 0;      //TRCD lower bridge enable
    TRISDbits.TRISD11 = 0;     //TRCD lower bridge
    TRISFbits.TRISF5 = 0;      //Lower voltage output, active low!
    TRISDbits.TRISD9 = 0;      //TRCD upper bridge
    TRISDbits.TRISD5 = 0;      //High voltage lower transistor

    TRISEbits.TRISE7 = 1;       //AD voltage measure high voltage in
    TRISBbits.TRISB0 = 1;       //TRDC2 INPUT
    TRISBbits.TRISB2 = 1;       //TRDC2 INPUT
    TRISBbits.TRISB3 = 0;       //TRDC1 INPUT
    TRISBbits.TRISB13 = 0;      //INPUT amplifier disable
    TRISBbits.TRISB14 = 0;      //INPUT amplifier disable
    LATBbits.LATB13 = 1;        //Disabled
    LATBbits.LATB14 = 1;        //Disabled
    TRISDbits.TRISD3 = 0;       //300V CHARGE DONE
    TRISDbits.TRISD4 = 0;       //300V CHARGE ENABLE
    TRISBbits.TRISB15 = 1;
    
    //Turn off HV lowering transistor (high = HV enabled)
    LATDbits.LATD0 = 1;
    LATDbits.LATD5 = 0;         
    LATDbits.LATD10 = 1;
    LATDbits.LATD11 = 0;
    
    LATCbits.LATC13 = 0;
    LATCbits.LATC14 = 0;
    LATEbits.LATE5 = 0;
    LATEbits.LATE6 = 0;
    LATGbits.LATG6 = 0;
    LATBbits.LATB13 = 0;        //Turn off AD-input diable (High = AD disabled)
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 1;

    TRISBbits.TRISB6 = 0;       //transducer disable input
    TRISBbits.TRISB7 = 0;       //transducer disable input
    TRISEbits.TRISE5 = 1;   //Alternative GPS PPS input
    TRISBbits.TRISB9 = 1; //GPS UART input
    TRISBbits.TRISB10 = 1; //GPS PPS input
    TRISBbits.TRISB11 = 1; //GPS PPS input
    TRISFbits.TRISF0 = 0; //GPS UART output //change to RB10 in later design.

    LATBbits.LATB6 = 1;
    LATBbits.LATB7 = 1;
    LATDbits.LATD9 = 0;
    LATDbits.LATD4 = 0;
    LATFbits.LATF5 = 1; //Lower voltage off

    /* initialize ADC calibration setting */
    // Turn off unused ADCs
    ANSELB = 0x00000000; 
    ANSELE = 0x00000000;
    ANSELG = 0x00000000;

    ANSELEbits.ANSE7 = 1;
    ANSELEbits.ANSE4 = 1;
    ANSELBbits.ANSB0 = 1;

    //CNENBbits.CNIEB10=1;
    //CNENBbits.CNIEB11=1;
    //CNENEbits.CNIEE5=1;
    //CNCONCSET = (1 << 15);
    LATEbits.LATE1 = 1; //Enable 1
    LATEbits.LATE2 = 1; //Enable 2
    /* Configure ADCCON1 */
    ADCCON1 = 0; // No ADCCON1 features are enabled including: Stop-in-Idle, turbo,
    // CVD mode, Fractional mode and scan trigger source.
    ADCCON1bits.SELRES = 3; // ADC7 resolution is 12 bits
    ADCCON1bits.STRGSRC = 1; // Select scan trigger.
    
    /* Configure ADCCON2 */
    ADCCON2bits.SAMC = 0; // ADC7 sampling time = 5 * TAD7
    ADCCON2bits.ADCDIV = 0; // ADC7 clock freq is half of control clock = TAD7

    /* Initialize warm up time register */
    ADCANCON = 0;
    ADCANCONbits.WKUPCLKCNT = 1; // Wakeup exponent = 32 * TADx
    /* Clock setting */
    ADCCON3bits.ADCSEL = 1; // Select input clock source
    ADCCON3bits.CONCLKDIV = 0; // Control clock frequency is half of input clock
    ADCCON3bits.VREFSEL = 1; // Select AVdd and AVss as reference source
    ADC0TIMEbits.ADCDIV = 0; // ADC0 clock frequency is half of control clock = TAD0
    ADC0TIMEbits.ADCEIS = 0; // ADC0 clock frequency is half of control clock = TAD0
    ADC0TIMEbits.SAMC = 0;  // ADC0 sampling time = 5 * TAD0 (changed from 0)
    ADC0TIMEbits.SELRES = 3; // ADC0 resolution is 12 bits
    /* Select analog input for ADC modules, no presync trigger, not sync sampling */
    ADCTRGMODEbits.SH0ALT = 0; // ADC0 = AN0
    /* Select ADC input mode */

    ADCIMCON1bits.SIGN0 = 0; // unsigned data format
    ADCIMCON1bits.DIFF0 = 0; // Single ended mode

    /* Configure ADCGIRQENx */
    ADCGIRQEN1 = 0; // No interrupts are used.
    ADCGIRQEN2 = 0;
    //ADCGIRQEN1bits.AGIEN10=1;
    /* Configure ADCCSSx */
    ADCCSS1 = 0; // Clear all bits
    ADCCSS1bits.CSS15 = 1; // AN15 (Class 3) set for scan
    ADCCSS1bits.CSS18 = 1; // AN18 (Class 3) set for scan

    /* Configure ADCCMPCONx */
    ADCCMPCON1 = 0; // No digital comparators are used. Setting the ADCCMPCONx
    ADCCMPCON2 = 0; // register to '0' ensures that the comparator is disabled.
    ADCCMPCON3 = 0; // Other registers are ?don't care?.
    ADCCMPCON4 = 0;
    ADCCMPCON5 = 0;
    ADCCMPCON6 = 0;
    /* Configure ADCFLTRx */
    ADCFLTR1 = 0; // No oversampling filters are used.
    ADCFLTR2 = 0;
    ADCFLTR3 = 0;
    ADCFLTR4 = 0;
    ADCFLTR5 = 0;
    ADCFLTR6 = 0;
    /* Early interrupt */
    ADCEIEN1 = 0; // No early interrupt
    ADCEIEN2 = 0;
    /* Turn the ADC on */
    ADCCON1bits.ON = 1;
    /* Wait for voltage reference to be stable */
    while (!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
    while (ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
    /* Enable clock to analog circuit */
    ADCANCONbits.ANEN0 = 1; // Enable the clock to analog bias ADC0
    ADCANCONbits.ANEN7 = 1; // Enable, ADC7

    /* Wait for ADC to be ready */
    while (!ADCANCONbits.WKRDY0); // Wait until ADC0 is ready
    while (!ADCANCONbits.WKRDY7); // Wait until ADC7 is ready
    /* Enable the ADC module */
    ADCCON3bits.DIGEN0 = 1; // Enable ADC0
    ADCCON3bits.DIGEN7 = 1; // Enable ADC7


    T1CON = 0x0; // Stop any 16-bit Timer5 operation
    T1CONSET = 0x0000; // Enable 32-bit mode, prescaler 1:1,
    // internal peripheral clock source
    TMR1 = 0x0; // Clear contents of the TMR4 and TMR5
    T1CONbits.TON = 0; // Stop Timer1
    // Enable during sleep/idle
    //T1CONbits.SIDL=0;

    T2CON = 0x0; // Stop any 16-bit Timer5 operation
    T2CONSET = 0x0000; // Enable 32-bit mode, prescaler 1:1,
    // internal peripheral clock source
    TMR2 = 0x0; // Clear contents of the TMR4 and TMR5
    T2CONbits.TON = 0; // Stop Timer1
    // Enable during sleep/idle
    T2CONbits.SIDL=0;

    T4CON = 0x0; // Stop any 16/32-bit Timer4 operation  / part of register control the pre-scaler
    T5CON = 0x0; // Stop any 16-bit Timer5 operation
    T4CONSET = 0x0008; // Enable 32-bit mode, prescaler 1:1,  // control the TMR4 setup
    // internal peripheral clock source
    TMR4 = 0x0; // Clear contents of the TMR4 and TMR5
    PR4 = 0xFFFFFFFF; // Load PR4 and PR5 registers with 32-bit value
    T4CONSET = 0x8000; // Start Timer4/5
    // Enable during sleep/idle
    T4CONbits.SIDL=0;
    
    // Setup timer 6/7 as a 32-bit timer
 //   T6CONSET = 0x0008; // Enable 32-bit mode, prescaler 1:1,    
//    T6CON = 0x0; // Stop any 16/32-bit Timer4 operation
//    T7CON = 0x0; // Stop any 16-bit Timer5 operation
//    TMR6 = 0x0; // Clear contents of the TMR6 and TMR7
//    PR6 = 0xFFFFFFFF; // Load PR4 and PR5 registers with 32-bit value
//    T6CONSET = 0x8000; // Start Timer6/7  
    
    // Scale up the adc conversion rate to avoid quantization when sampling
    ADCCON3bits.CONCLKDIV = 10;  // ADC Control Clock <Tq> Divider (Divide by 10)
    
}

// Initiate ???
void init_pps(void) {
    SYSKEY = 0x0;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    PB4DIV = 0x00008000;
    CFGCONbits.IOLOCK = 0;

    INT4Rbits.INT4R = 0b0110; // Set INT4 to RPE3

    U1RXRbits.U1RXR = 0b0000; // Set RX1 to RPD2
    RPD3Rbits.RPD3R = 0b0001; // Set TX1 to RPD3

    U5RXRbits.U5RXR = 0b0101; // Set RX1 to RPB9
    RPF0Rbits.RPF0R = 0b0011; // Set TX1 to RPF0       Has to change to RB11 later!

    CFGCONbits.IOLOCK = 1;

      SYSKEY = 0x33333333;
//    SYSKEY = 0x0;
//    SYSKEY = 0xAA996655;
//    SYSKEY = 0x556699AA;
//    OSCCONbits.SLPEN=1;
//    SYSKEY = 0x0;

}