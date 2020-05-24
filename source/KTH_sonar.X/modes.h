/* 
 * File:   modes.h
 * Author: vikto
 *
 * Created on December 7, 2017, 5:55 PM
 */

#ifndef MODES_H
#define	MODES_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct Mode{
        char state;
    } Mode;

Mode get_Mode_debug(void);
void set_Mode_debug(char state);

Mode get_Mode_debug_r(void);
void set_Mode_debug_r(char state);
    
Mode get_Mode_detector(void);
void set_Mode_detector(char state);
void toggle_Mode_detector(void);

Mode get_Mode_switcher(void);
void set_Mode_switcher(char state);
   

Mode get_Mode_autoset_voltage(void);
void set_Mode_autoset_voltage(char state);
    
Mode get_Mode_autoping(void);
void set_Mode_autoping(char state);

Mode get_Mode_detect_waveform(void);
void set_Mode_detect_waveform(char state);

//Mode get_Mode_rawoutput(void);
//void set_Mode_rawoutput(char state);

Mode get_Mode_ramping_up(void);
void set_Mode_ramping_up(char state);

Mode get_Mode_ramping_down(void);
void set_Mode_ramping_down(char state);

Mode get_Mode_idle(void);
void set_Mode_idle(char state);

void toggle_Mode_debug(void);
void toggle_Mode_debug_r(void);
void toggle_Mode_switcher(void);

void toggle_Mode_autoping(void);
void toggle_Mode_rawoutput(void);
void toggle_Mode_idle(void);
void toggle_Mode_detect_waveform(void);

Mode get_Mode_human_interface(void);
void set_Mode_human_interface(char state);

char toggle(char state);

#ifdef	__cplusplus
}
#endif

#endif	/* MODES_H */

