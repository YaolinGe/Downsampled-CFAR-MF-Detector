/* 
 * File:   commands.h
 * Author: vikto
 *
 * Created on December 17, 2017, 7:27 PM
 */

#ifndef COMMANDS_H
#define	COMMANDS_H

#ifdef	__cplusplus
extern "C" {
#endif

void cmnd_load_file(void);
void cmnd_set_comdelay(char *data);
void cmnd_set_frequency(char *data);
void cmnd_set_mf_detector_threshold(char *data,int cmdlen);
void cmnd_set_number_of_pulses(char *data,int cmdlen);
void cmnd_set_voltage(char *data);
void cmnd_set_storage_vector_len(char *data,int cmdlen);
void cmnd_set_soundspeed(char *data);
void cmnd_set_rawmode(void);
void cmnd_set_apmode(void);
void cmnd_send_square_pulse(void);
void cmnd_send_chirp_pulse(void);
void cmnd_print_sonar_settings(int cmdlen);
void cmnd_debug_mode(void);
void cmnd_debug(void);
void cmnd_debug_r(void);
void cmnd_set_debug_var(char *data,int cmdlen);
void cmnd_set_mf_detector_threshold(char *data, int cmdlen);
void cmnd_set_amp_detector_threshold(char *data, int cmdlen);
void cmnd_set_detector_mode(void);
void cmnd_detect_waveform(void);
void cmnd_send_pulse_listen(void);
void cmnd_send_pulse_listen_correlate(void);
void cmnd_set_comdelay(char *data);
void cmnd_activate_GPS(void);
void cmnd_deactivate_GPS(void);
void cmnd_activate_GPS_feedthrough(void);
void cmnd_deactivate_GPS_feedthrough(void);
void cmnd_show_GPS_time(void);
void cmnd_show_GPS_fix(void);
void cmnd_show_GPS_location(void);
void cmnd_activate_timecalibration(void);
void cmnd_activate_cyclic_output(void);
void cmnd_deactivate_cyclic_output(void);
void cmnd_download_storage_vector(void);
void cmnd_download_correlation_vector(void);
void cmnd_download_time_vector(void);
void cmnd_download_correlation_waveform(void);
void cmnd_set_thrscale_nom(char *data,int cmdlen);
void cmnd_set_thrscale_reverb(char *data,int cmdlen);
void cmnd_stop_auto_ping(void);

// yaolin debug
unsigned int get_load_num(void);
void cmnd_set_debug_r_mode(void);
void cmnd_set_fixed_threshold(char *data);
void cmnd_clear_everything(void);
void cmnd_set_scale_factor(char *data);
void cmnd_set_switcher_mode(void);
void cmnd_set_start_delay(char *data);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMANDS_H */

