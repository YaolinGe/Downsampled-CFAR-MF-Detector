#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include "main.h"
#include "setup.h"
#include "PIC32config.h"
#include "SONARconfig.h"
#include "flags.h"
#include "modes.h"
#include "sonar.h"
#include "uplink.h"
#include "detection.h"
#include "downlink.h"
#include "georeference.h"
#include "delay.h"

// used to debug
#include "time.h"
#include "pulse.h"
#include "sampling.h"



/* Main **************************************************************
 * Main keeps control of the program flow, by holding all flag variables. 
 * It takes commands in receiveCommand(), updates and packet the flags,
 * and operates the sonar with the set instructions.
 */
/************************/

float ts_AP=1/MAX_FREQ_AUTOPING; // [s] wait time between autopings 
char wait=1;

/* 32-bit timer used for auto-pinging (uses 16-bit timers 6 & 7) */
void __attribute__((interrupt(IPL2AUTO), vector(_TIMER_9_VECTOR)))
Timer9Handler(void);
void Timer9Handler(void) {    
    wait=0; // Stop waiting, fire off a ping
    IFS1bits.T9IF = 0;
}

Time t_current;
Time t_previous;

/*
 Main: Checks for new commands (that set flags or operation modes), 
 * operates the sonar according to current flags/operation modes, 
 * and clears flags after one operation cycle. A timer will keep 
 * track of when to send pulses if the sonar is set in autoping 
 * mode. 
 */
int32_t main(void)
{
    // Start of sonar, run setup code
    setup();  
    
    //LATBbits.LATB3 = 1;
    //delaymilli(1000);

    // Start operation
	do{
        // Check if autopinging should be started or stopped
//        if( (get_Mode_autoping().state == 1) && (get_Mode_idle().state==1) ){
//            set_Mode_idle(FALSE);
//            //set_Mode_rawoutput(FALSE); // Cannot be enabled with autoping
//            start_AP();
//        }
//        else if ( (get_Mode_autoping().state == 0) && (get_Mode_idle().state==0) ){
//            set_Mode_idle(TRUE);
//            stop_AP();
//        }
        test_AP_DBR();
//        test_beacon();
//        test_range_finder();


        //asm volatile ( "wait" ); // Put device in selected power-saving mode    
        // Normal operation --------------------------
        receiveCommand();   // Check serial communication for commands
        operateSonar();     // Do one operation cycle of sonar (listen-pulse-detect)
        clear_flags();      // Flags are cleared each program cycle

        if (get_GPSact() == 1){
            operateGPS();
        }
        // -------------------------------------------
        

        // Set flag to send a pulse, used with autoping timer
        if( wait!=TRUE ){
//            printString("autoping is on!"); printEL();
            set_Flag_cmd_send_pulse(TRUE);
            wait=1;
        }  

    } while (1);
   
	return(1);
}

void test_AP_DBR(void)
{
    if (get_Mode_switcher().state == 1)
    {
//        printString("It is here!"); printEL();
        set_Mode_debug_r(FALSE);
        if( (get_Mode_autoping().state == 1) && (get_Mode_idle().state==1))
        {
            set_Mode_idle(FALSE);
            printString("start autoping!"); printEL();
            printString("frequency is "); printInt(get_frequency()); printEL();

            //set_Mode_rawoutput(FALSE); // Cannot be enabled with autoping
            start_AP();
            printString("1"); printEL();
        }
        else 
        if ((get_Mode_autoping().state == 0) && (get_Mode_idle().state==0))
        {
            set_Mode_idle(TRUE);
            stop_AP();
            printString("2"); printEL();
        }
    }
    if (get_Mode_switcher().state == 0)
    {
//        printString("haha"); printEL();
        set_Mode_autoping(FALSE);
        if( (get_Mode_debug_r().state == 1) && (get_Mode_idle().state==1) ){
            set_Mode_idle(FALSE);
            start_debug_r();
//            test_beacon();
            printString("3"); printEL();
        }
        else 
        if ((get_Mode_debug_r().state == 0) && (get_Mode_idle().state==0)){
            set_Mode_idle(TRUE);
            stop_debug_r();
            printString("4"); printEL();
        }
    }
}

void test_beacon(void)
{
//    printString("beacon is here!"); printEL();
    // here it should be a detector to find the exceedance
    
    // at the same time, the timing clock should be activated
    
    // here it should be the pulsing part, once the trigger delay is actvated
    
    beacon();
//    while(TRUE)
//    {
//        int i = beacon();
//        if (i)
//        {
//            break;
//        }
//        break;
//    }

}

void test_range_finder(void)
{
    if (get_Mode_switcher().state == TRUE)
    {
        print_val("exceedance is ", get_exceedance());
        if(get_exceedance())
        {
            mf_detector_setup(70000);
            t_current.clk = get_time_clk();
//            set_time_current(get_time_clk());
            t_previous = get_time_current();
            printString("the time difference is "); printDouble(get_time_diff(t_current, t_previous)); printString("secs"); printEL();
            
//            set_time_current();
            send_pulse(70000, SINE, 0);
            printString("Hello, pulse is sent! "); printEL();
            
            set_Mode_switcher(FALSE);
            set_Mode_detector(FALSE);
        }
    }
    if (get_Mode_switcher().state == FALSE)
    {
        if( (get_Mode_detector().state == TRUE) && (get_Mode_idle().state == TRUE))
        {
            mf_detector_setup(47000);
            set_Mode_idle(FALSE);
            printString("3"); printEL();
            test_mf_detector(1);
        }
        else if( (get_Mode_detector().state == FALSE) && (get_Mode_idle().state == FALSE))
        {
            set_Mode_idle(TRUE);
            printString("4"); printEL();
            printString("stop here!"); printEL();
//            test_mf_detector(2);
        }
    }
}

void start_debug_r(void)
{
    ini_everything();
    printString("DBR IS ON"); printDelim(); printEL();
//    delaymilli(1000);
    int Nframes = 500;
//    test_pre_filtering_with_fixed_threshold(Nframes);
//    test_pre_filtering_with_adapted_threshold(Nframes);
//    test_matched_filtering_with_fixed_threshold_dc(Nframes);
    test_matched_filtering_with_adapted_threshold_dc(Nframes);
}

void stop_debug_r(void)
{
    delaymilli(1000);
    printString("DEBUG IS CANCELLED"); printDelim(); printEL();
}

/* Start autopinging, used when set to autoping mode */
void start_AP(void)
{
   // Limit ping to once every 1-30 seconds
    if(ts_AP<(float)1/MAX_FREQ_AUTOPING){ts_AP=(float)1/MAX_FREQ_AUTOPING;}
    else if (ts_AP>30){ts_AP=30;} 
    float scaler=ts_AP*FCY; // Set appropriate scaler for autoping time

//    Time tpulse;
//    Time tlisten;
//    tpulse.clk=get_t_pulsestart();

    // Ramp up voltage until sonar gets at least one hit
    // dump_voltage(); // This needs to be tested
    set_consecutive_hits(0);
    set_consecutive_misses(0);
    //set_Mode_ramping_up(TRUE);
    //set_Mode_autoset_voltage(TRUE);
    //set_voltage(MIN_VOLTAGE); // Start ramping up from V=min_voltage

    // 32 bit mode! Timer8 and Timer9 are combined, timer operations are 
    // performed by Timer8, and interrupt functions by Timer9
    T8CONbits.TON = 0; // Stop Timer6
    T8CONbits.TCS=0; // Choose internal clock instead of external (default)

    T8CONbits.T32=1; // Combine Timer8 and Timer9 into a 32-bit timer)
    T8CONbits.TCKPS = 0b000; // System clock divider 32bit mode   

    PR8=(uint32_t)scaler; // Pre-scaler (interrupt when register has counted to the value of PR6)

    IFS1bits.T9IF=0; // Clear interrupt flag
    IPC10bits.T9IP = 2; // Set priority 2 

    IEC1bits.T9IE = 1; // Enable interrupt on Timer 6/7
    T8CONbits.TON = 1; // Activate Timer6/7 

    /*
    // 32 bit mode! Timer6 and Timer7 are combined, timer operations are 
    // performed by Timer6, and interrupt functions by Timer7
    T6CONbits.TON = 0; // Stop Timer6
    T6CONbits.TCS=0; // Choose internal clock instead of external (default)
    T6CONbits.T32=1; // Combine Timer6 and Timer7 into a 32-bit timer)
    T6CONbits.TCKPS = 0b000; // System clock divider 32bit mode   
    PR6=(uint32)scaler; // Pre-scaler (interrupt when register has counted to the value of PR6)
    IFS1bits.T7IF=0; // Clear interrupt flag
    IPC8bits.T7IP = 2; // Set priority 2 
    IEC1bits.T7IE = 1; // Enable interrupt on Timer 6/7
    T6CONbits.TON = 1; // Activate Timer6/7 
    */

//    tlisten.clk=get_time_clk();
//    printInt(clk2sec(tlisten.clk));printDelim();printEL();
}

/* Stop autopinging, used when autoping mode is turned off*/
void stop_AP(void){
    /*IEC1bits.T7IE = 0; // Disable interrupt 
    IFS1bits.T7IF=0; // Clear interrupt flag
    T6CONbits.TON = 0; // Stop Timer6/7 
    T6CON=0; // Clear control registers for timer 6/7
    T7CON=0;
    set_Mode_ramping_up(FALSE);
    set_Mode_autoset_voltage(FALSE);*/
    
    IEC1bits.T9IE = 0; // Disable interrupt 
    IFS1bits.T9IF=0; // Clear interrupt flag
    T8CONbits.TON = 0; // Stop Timer6/7 
    T8CON=0; // Clear control registers for timer 6/7
    T9CON=0;
    set_Mode_ramping_up(FALSE);
    set_Mode_autoset_voltage(FALSE);
}
