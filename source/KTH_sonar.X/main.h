/* 
 * File:   main.h
 * Author: Viktor
 *
 * Created on den 24 juli 2017, 12:15
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif
        
char get_flag_isdetected(void);     
char get_mode_autoping(void);
char get_mode_raw(void);      
char get_mode_avgrange(void);
char get_mode_debug(void);
char get_trigger_varinforequest(void);
char get_trigger_pulsetrain(void);
char get_trigger_ping_rawlisten(void);
void start_AP(void);
void stop_AP(void);


void start_debug_r(void);
void stop_debug_r(void);
void test_AP_DBR(void);
void test_range_finder(void);// if the signal is detected, then send a different pulse and listen to find the range
void test_beacon(void);




#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

