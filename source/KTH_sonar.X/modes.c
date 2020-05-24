#include "modes.h"
#include "SONARconfig.h"
/* Operation modes for the sonar:
 * 
 * Mode autoping: Send pulse and measure depth at a 
 * steady interval. 
 * 
 * Mode rawoutput: Tells the sonar to transmit the buffer 
 * after an echo has been received. Can only be used with 
 * commands that send a single pulse.
 * 
 * Mode idle: Baseline for the sonar, when it is only 
 * waiting for a command (default mode at start).
 * 
 * Mode ramping_up: In this mode, the sonar is ramping up 
 * the amplitude (voltage) of the pulse, after each 
 * consecutive pulse (used together with autoping mode).
 * 
 * Mode autoset_voltage: In this mode, the sonar is keeping 
 * track of consecutive detected and missed echos, in order 
 * to determine if it should ramp up voltage(amplitude) or keep 
 * it at the same level. Used together with the modes autoping and 
 * ramping_up.
 *  
 */

// Variables for containing the state of each mode
Mode debug;
Mode debug_r;
Mode detector;

Mode switcher;
Mode autoping;
Mode detect_waveform;
//Mode rawoutput;
Mode idle;
Mode ramping_up; // Ramping up voltage
Mode ramping_down;
Mode autoset_voltage;
Mode human_interface; // For printing messages as codes or text

// Getters and setters for Mode variables
Mode get_Mode_debug(void){
    return debug;
}

Mode get_Mode_debug_r(void){
    return debug_r;
}

Mode get_Mode_detector(void){
    return detector;
}

Mode get_Mode_switcher(void){
    return switcher;
}
void set_Mode_debug(char state){
    debug.state=state;
}

void set_Mode_debug_r(char state){
    debug_r.state=state;
}

void set_Mode_detector(char state){
    detector.state=state;
}

void set_Mode_switcher(char state){
    switcher.state=state;
}

Mode get_Mode_autoset_voltage(void){
    return autoset_voltage;
}
void set_Mode_autoset_voltage(char state){
    autoset_voltage.state=state;
}
Mode get_Mode_autoping(void){
    return autoping;
}
void set_Mode_autoping(char state){
    autoping.state=state;
}

Mode get_Mode_detect_waveform(void){
    return detect_waveform;
}
void set_Mode_detect_waveform(char state){
    detect_waveform.state=state;
}
/*Mode get_Mode_rawoutput(void){
    return rawoutput;
}
void set_Mode_rawoutput(char state){
    rawoutput.state=state;    
}*/
Mode get_Mode_idle(void){
    return idle;
}
void set_Mode_idle(char state){
    idle.state=state;
}
Mode get_Mode_ramping_up(void){
    return ramping_up;
}
void set_Mode_ramping_up(char state){
    ramping_up.state=state;
    if(state==TRUE){
        ramping_down.state=!state; // Can't ramp up and down at the same time
    }
}
Mode get_Mode_human_interface(void){
    return human_interface;
}
void set_Mode_human_interface(char state){
    human_interface.state=state;
}

Mode get_Mode_ramping_down(void){
    return ramping_down;
}
void set_Mode_ramping_down(char state){
    ramping_down.state=state;
    if(state==TRUE){
        ramping_up.state=!state; // Can't ramp up and down at the same time
    }
}

// Toggle state of a mode
void toggle_Mode_debug(void){
    debug.state=toggle(debug.state);
}

void toggle_Mode_debug_r(void){
    debug_r.state=toggle(debug_r.state);
}

void toggle_Mode_detector(void){
    detector.state=toggle(detector.state);
}

void toggle_Mode_autoping(void){
    autoping.state=toggle(autoping.state);
}

void toggle_Mode_switcher(void){
    switcher.state=toggle(switcher.state);
}

void toggle_Mode_detect_waveform(void){
    detect_waveform.state=toggle(detect_waveform.state);
}
/*void toggle_Mode_rawoutput(void){
    rawoutput.state=toggle(rawoutput.state);
}*/
void toggle_Mode_idle(void){
    idle.state=toggle(idle.state);
}

// Toggle state of a variable
char toggle(char state) {
    return 1 - state;
}