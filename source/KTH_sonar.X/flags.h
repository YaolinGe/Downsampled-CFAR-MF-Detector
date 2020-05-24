/* 
 * File:   flags.h
 * Author: vikto
 *
 * Created on December 11, 2017, 5:16 PM
 */

#ifndef FLAGS_H
#define	FLAGS_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct Flag{
        char state;
    } Flag;

    
void set_Flag_detection(char state);
//void set_Flag_cmd_parameter_request(char state);
void set_Flag_cmd_send_pulse(char state);
void set_Flag_cmd_send_pulse_listen(char state);
void set_Flag_cmd_send_pulse_listenc(char state);
void set_Flag_cmd_debug(char state);
void set_Flag_cmd_debug_r(char state);
void set_Flag_saturation(char state);

Flag get_Flag_detection(void);
Flag get_Flag_saturation(void);
//Flag get_Flag_cmd_parameter_request(void);
Flag get_Flag_cmd_send_pulse(void);
Flag get_Flag_cmd_send_pulse_listen(void);
Flag get_Flag_cmd_send_pulse_listenc(void);
Flag get_Flag_cmd_debug(void);
Flag get_Flag_cmd_debug_r(void);
Flag get_Flag_saturation(void);

void clear_flags(void);

#ifdef	__cplusplus
}
#endif

#endif	/* FLAGS_H */

