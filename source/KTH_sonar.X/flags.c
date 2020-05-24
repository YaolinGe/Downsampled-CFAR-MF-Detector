#include "flags.h"
#include "SONARconfig.h"

/* 
 * Flags that are triggered by either a command or some part of the code. 
 * They are cleared after each program cycle!
 * 
 * Flag detection: The detector has detected an echo.
 * 
 * Flag cmd_parameter_request: Command received to 
 * transmit a selection of sonar parameters on the downlink.
 * 
 * Flag cmd_send_pulsetrain: Command received to send a single pulse. 
 * 
 * Flag cmd_send_pulsetrain_longlisten; Command received to send a single 
 * pulse, and sample at max frequency from start of pulse until end of 
 * buffer length.
 * 
 * Flag cmd_debug; Command received to run defined debug code. 
 * 
 */

Flag detection;
Flag saturation;
//Flag cmd_parameter_request;
Flag cmd_send_pulse;
Flag cmd_send_pulse_listen;
Flag cmd_send_pulse_listenc;
Flag cmd_debug;
Flag cmd_debug_r;


// Getters and setters for flag states
void set_Flag_detection(char state){
    detection.state=state;
}
void set_Flag_saturation(char state){
    saturation.state=state;
}
/*void set_Flag_cmd_parameter_request(char state){
    cmd_parameter_request.state=state;
}*/
void set_Flag_cmd_send_pulse(char state){
    cmd_send_pulse.state=state;
}
void set_Flag_cmd_send_pulse_listen(char state){
    cmd_send_pulse_listen.state=state;
}
void set_Flag_cmd_send_pulse_listenc(char state){
    cmd_send_pulse_listenc.state=state;
}
void set_Flag_cmd_debug(char state){
    cmd_debug.state=state;
}

void set_Flag_cmd_debug_r(char state){
    cmd_debug_r.state=state;
}

Flag get_Flag_detection(void){
    return detection;
}
Flag get_Flag_saturation(void){
    return saturation;
}
Flag get_Flag_cmd_send_pulse(void){
    return cmd_send_pulse;
}
Flag get_Flag_cmd_send_pulse_listen(void){
    return cmd_send_pulse_listen;
}
Flag get_Flag_cmd_send_pulse_listenc(void){
    return cmd_send_pulse_listenc;
}
Flag get_Flag_cmd_debug(void){
    return cmd_debug;
}

Flag get_Flag_cmd_debug_r(void){
    return cmd_debug_r; // receiver debug mode
}

// Set all flags to false. They are cleared after each program cycle.
void clear_flags(void){
set_Flag_detection(FALSE);
set_Flag_saturation(FALSE);
set_Flag_cmd_send_pulse(FALSE);
set_Flag_cmd_send_pulse_listen(FALSE);
set_Flag_cmd_send_pulse_listenc(FALSE);
set_Flag_cmd_debug(FALSE);
set_Flag_cmd_debug_r(FALSE);
}