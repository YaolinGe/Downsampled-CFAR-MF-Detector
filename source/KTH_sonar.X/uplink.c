#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include "commands.h"
#include "downlink.h"
#include "detection.h"

/*****************************************************
 * Processes letters in the communication buffer, and 
 * looks for letter combinations that constitute a command.
 */
void receiveCommand(void) {
    if (U1STAbits.URXDA != 0) {
        int timeout = 0;
        char addr[3];  // id storage vector  
        char data[40];
        //char value[5];
        int iic = 0;
        char inchar = U1RXREG;
        if (inchar == '$') {
            timeout = 0;
            do {
                timeout++; //
            } while (U1STAbits.URXDA == 0 && timeout < 10000); ///why it needs 1e5times
            // Transpose from ascii to number
            addr[0] = U1RXREG - 48;
            timeout = 0;
            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);
            addr[1] = U1RXREG - 48;
            timeout = 0;
            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);
            addr[2] = U1RXREG - 48;
            timeout = 0;
            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);
            inchar = U1RXREG; //receive the last '$'
            // Any unit id works? 
            if (addr[0] * 100 + addr[1] * 10 + addr[2] * 1 < 999 && addr[0] * 100 + addr[1] * 10 + addr[2] * 1 >= 0) {
                memset(data, 0, sizeof (data)); //clear string
                do {
                    timeout = 0;
                    do {
                        timeout++;
                    } while (U1STAbits.URXDA == 0 && timeout < 1000);
                    data[iic] = U1RXREG;
                    iic++;
                } while (iic < 35 && timeout < 998 && data[iic] != '$');
                
                if (iic < 30) {
                    int cmdlen;
                    // Download storage vector
                    if (data[0] == 'D' && data[1] == 'S'
                            && data[2] == 'V') {
                        cmnd_download_storage_vector();
                        goto label_CommandReceived;
                    }
                    // Download correlation vector for last echo
                    else if (data[0] == 'D' && data[1] == 'C'
                            && data[2] == 'V') {
                        cmnd_download_correlation_vector();
                        goto label_CommandReceived;
                    }
                    // Download time vector for last echo
                    else if (data[0] == 'D' && data[1] == 'T'
                            && data[2] == 'V') {
                        cmnd_download_time_vector();
                        goto label_CommandReceived;
                    }
                    // Download time vector for last echo
                    else if (data[0] == 'P' && data[1] == 'L'
                            && data[2] == 'C') {
                        cmnd_send_pulse_listen_correlate();
                        goto label_CommandReceived;
                    }
                    // Set number of pulses
                    else if (data[0] == 'S' && data[1] == 'N'
                            && data[2] == 'P') {
                        cmdlen=3;
                        cmnd_set_number_of_pulses(data,cmdlen);
                        goto label_CommandReceived; 
                    }
                    // Set length of raw adc vector
                    else if (data[0] == 'S' && data[1] == 'S'
                            && data[2] == 'L') {
                        cmdlen=3;
                        cmnd_set_storage_vector_len(data,cmdlen);
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'T' && data[1] == 'H'
                            && data[2] == 'R') {
                        cmdlen=3;
                        cmnd_set_thrscale_reverb(data,cmdlen);
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'T' && data[1] == 'H'
                            && data[2] == 'N') {
                        cmdlen=3;
                        cmnd_set_thrscale_nom(data,cmdlen);
                        goto label_CommandReceived;
                    }
                    // Request information on variable values
                    else if (data[0] == 'P' && data[1] == 'S' 
                            && data[2] == 'S') {
                        cmdlen=3;
                        cmnd_print_sonar_settings(cmdlen);
                        goto label_CommandReceived;
                    }
                    // Set value on variable that you want to debug 
                    // (is done in void cmnd_set_debugvariable(char *data) 
                    // in uplink.c )
                    else if (data[0] == 'S' && data[1] == 'D'
                            && data[2] == 'V') {
                        cmdlen=3;
                        cmnd_set_debug_var(data,cmdlen);
                        goto label_CommandReceived;
                    }
                    // Send square pulse
                    else if (data[0] == 'S' && data[1] == 'Q'
                            && data[2] == 'P') {
                        cmnd_send_square_pulse();
                        set_wave_select(SINE);
                        goto label_CommandReceived;
                    }
                    
                    // Set square pulse
                    else if (data[0] == 'S' && data[1] == 'W'
                            && data[2] == 'S') {
                        // Set waveform: sine
                        set_wave_select(SINE);
                        printInt(get_wave_select()); printEL();
                        goto label_CommandReceived;
                    }
                    // Set square pulse
                    else if (data[0] == 'S' && data[1] == 'W'
                            && data[2] == 'C') {
                        // Set waveform: chirp
                        set_wave_select(CHIRP);
                        printInt(get_wave_select()); printEL();
                        goto label_CommandReceived;
                    }
                    
                    // Send sine pulse
                    else if (data[0] == 'S' && data[1] == 'I'
                            && data[2] == 'P') {
                        printString("Sine pulse not implemented");printEL();
                        goto label_CommandReceived;
                    }
                    // Send chirp pulse
                    else if (data[0] == 'C' && data[1] == 'H'
                            && data[2] == 'P') {
                        set_wave_select(CHIRP);
                        cmnd_send_chirp_pulse();
                        //printString("Chirp pulse not implemented");printEL();
                        goto label_CommandReceived;
                    }
                    // Send pulse and listen for a predetermined time
                    else if (data[0] == 'P' && data[1] == 'L') {
                        cmnd_send_pulse_listen();
                        goto label_CommandReceived;
                    }   
                    /*else if (data[0] == 'M' && data[1] == 'T'
                            && data[2] == 'R') {
                        cmdlen=3;
                        cmnd_set_mf_detector_threshold(data,cmdlen);
                        goto label_CommandReceived;
                    }*/
                    else if (data[0] == 'A' && data[1] == 'T'
                            && data[2] == 'R') {
                        cmdlen=3;
                        cmnd_set_amp_detector_threshold(data,cmdlen);
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'C' && data[1] == 'L'
                            && data[2] == 'R') {
                        cmnd_clear_everything();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'D' && data[1] == 'C'
                            && data[2] == 'W') {
                        cmnd_download_correlation_waveform();
                        goto label_CommandReceived;
                    }
                    
                    
                    // Run sonar in debug mode (sample from storage vector
                    // instead of from the adc)
                    else if (data[0] == 'D' && data[1] == 'B'
                            && data[2] == 'M') {
                        cmnd_debug_mode();
                        goto label_CommandReceived; 
                    }
                    else if (data[0] == 'D' && data[1] == 'B'
                            && data[2] == 'R') {
                        cmnd_set_debug_r_mode();
                        goto label_CommandReceived; 
                    }
                    else if (data[0] == 'S' && data[1] == 'T'
                            && data[2] == 'R') {
                        cmnd_set_fixed_threshold(data);
                        goto label_CommandReceived; 
                    }
                    else if (data[0] == 'S' && data[1] == 'S'
                            && data[2] == 'D') {
                        cmnd_set_start_delay(data);
                        goto label_CommandReceived; 
                    }
                    else if (data[0] == 'S' && data[1] == 'S'
                            && data[2] == 'F') {
                        cmnd_set_scale_factor(data);
                        goto label_CommandReceived; 
                    }

                    
                    else if (data[0] == 'D' && data[1] == 'B') {
                        cmnd_debug();
                        goto label_CommandReceived;
                    } 
                    else if (data[0] == 'L' && data[1] == 'F') {
                        cmnd_load_file();
                        goto label_CommandReceived;
                    }                    
                    else if (data[0] == 'S' && data[1] == 'T') {
                        cmnd_set_comdelay(data);
                        goto label_CommandReceived;
                    }
                    // Set frequency
                    else if (data[0] == 'S' && data[1] == 'F') {
                        cmnd_set_frequency(data);
                        goto label_CommandReceived;
                    }
                    // Set soundspeed (speed of of the medium)
                    else if (data[0] == 'S' && data[1] == 'P') {
                        cmnd_set_soundspeed(data);
                        goto label_CommandReceived;
                    }
                    // Activate auto-ping mode
                    else if (data[0] == 'A' && data[1] == 'P') {
                        cmnd_set_apmode();
                        goto label_CommandReceived;
                    }
                    // Set voltage 
                    else if (data[0] == 'S' && data[1] == 'V') {
                        cmnd_set_voltage(data);
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'D' && data[1] == 'W') {
                        cmnd_detect_waveform();
                        goto label_CommandReceived;
                    }
                    else  if (data[0] == 'S' && data[1] == 'O') {
                        cmnd_stop_auto_ping();
                        goto label_CommandReceived;
                    }  
                    else  if (data[0] == 'S' && data[1] == 'W') {
                        cmnd_set_switcher_mode();
                        goto label_CommandReceived;
                    }      
                    else  if (data[0] == 'D' && data[1] == 'M') {
                        cmnd_set_detector_mode();
                        goto label_CommandReceived;
                    }  
                    

                    
                    // GPS related commands
                    else if (data[0] == 'A' && data[1] == 'D') {
                        char aa, bb, cc;
                        aa = data[2] - 48;
                        bb = data[3] - 48;
                        cc = data[4] - 48;
                        if (aa * 100 + bb * 10 + cc * 1 < 999 && aa * 100 + bb * 10 + cc * 1 >= 0) {
                            //setaddress(aa, bb, cc);
                        }
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'S' && data[1] == 'D') {
                        cmnd_set_comdelay(data);
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'A') {
                        cmnd_activate_GPS();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'I') {
                        cmnd_deactivate_GPS();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'F') {
                        cmnd_activate_GPS_feedthrough();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'X') {
                        cmnd_deactivate_GPS_feedthrough();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'T') {
                        cmnd_show_GPS_time();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'U') {
                        cmnd_show_GPS_fix();
                        goto label_CommandReceived;
                    }
                    else  if (data[0] == 'G' && data[1] == 'L') {
                        cmnd_show_GPS_location();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'G' && data[1] == 'C') {
                        cmnd_activate_timecalibration();
                        goto label_CommandReceived;
                    }
                    else if (data[0] == 'C' && data[1] == 'C') {
                        cmnd_activate_cyclic_output();
                        goto label_CommandReceived;
                    }
                    else  if (data[0] == 'C' && data[1] == 'X') {
                        cmnd_deactivate_cyclic_output();
                        goto label_CommandReceived;
                    }
            
                    

                }

            }

        }
    }
label_CommandReceived:
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    if (U1STAbits.PERR == 1) {
        U1STAbits.PERR = 0;
    }
    Nop();
}
/*
void receiveCommand(void) {

    if (U1STAbits.URXDA != 0) {

        int timeout = 0;
        char addr[3];
        char data[40];
        int iic = 0;
        char inchar = U1RXREG;
        timeout = 0;
        if (inchar == '$') {

            timeout = 0;
            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);

            addr[0] = U1RXREG - 48;
            timeout = 0;

            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);

            addr[1] = U1RXREG - 48;
            timeout = 0;

            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);

            addr[2] = U1RXREG - 48;
            timeout = 0;

            do {
                timeout++;
            } while (U1STAbits.URXDA == 0 && timeout < 10000);

            inchar = U1RXREG; //receive the last '$'

            if (addr[0] * 100 + addr[1] * 10 + addr[2] * 1
                    < 999 && addr[0] * 100 + addr[1] * 10 + addr[2] * 1
                    >= 0) {
                memset(data, 0, sizeof (data)); //clear string
                do {
                    timeout = 0;
                    do {
                        timeout++;
                    } while (U1STAbits.URXDA == 0 && timeout < 1000);
                    data[iic] = U1RXREG;
                    iic++;
                } while (iic < 35 && timeout < 998 && data[iic] != '$');

                if (iic < 30) {

                    // Set value of startdelay
                    if (data[0] == 'S' && data[1] == 'T') {
                        cmnd_set_startdelay(data);
                        goto label_CommandReceived;
                    }
                    // Set frequency
                    if (data[0] == 'S' && data[1] == 'F') {
                        cmnd_set_frequency(data);
                        goto label_CommandReceived;
                    }
                    // Set output to RAW
                    if (data[0] == 'O' && data[1] == 'R') {
                        cmnd_set_rawmode();
                        goto label_CommandReceived;
                    }
                    // Send pulsetrain
                    if (data[0] == 'P' && data[1] == 'T') {
                        cmnd_send_pulsetrain(data);
                        goto label_CommandReceived;
                    }
                    
                    // Send pulsetrain and listen in raw mode
                    if (data[0] == 'P' && data[1] == 'R') {
                        cmnd_ping_rawlisten();
                        goto label_CommandReceived;
                    }
                    
                    // Set soundspeed (speed of of the medium)
                    if (data[0] == 'S' && data[1] == 'P') {
                        cmnd_set_soundspeed(data);
                        goto label_CommandReceived;
                    }
                    // Set exceedance threshold for echo detection
                    if (data[0] == 'T' && data[1] == 'E') {
                        cmnd_set_threshexc(data);
                        goto label_CommandReceived;
                    }
                    // Activate auto-ping mode
                    if (data[0] == 'A' && data[1] == 'P') {
                        cmnd_set_apmode();
                        goto label_CommandReceived;
                    }
                    // Set threshold for matched filter detection
                    if (data[0] == 'M' && data[1] == 'F') {
                        cmnd_set_mfthresh(data);
                        goto label_CommandReceived;
                    }
                    // Set voltage tolerance
                    if (data[0] == 'V' && data[1] == 'T') {
                        cmnd_set_voltagetolerance(data);
                        goto label_CommandReceived;
                    }
                    // Set voltage 
                    if (data[0] == 'S' && data[1] == 'V') {
                        cmnd_set_voltage(data);
                        goto label_CommandReceived;
                    }

                    // Set length of raw adc vector
                    if (data[0] == 'R' && data[1] == 'L') {
                        cmnd_set_raw_adc_len(data);
                        goto label_CommandReceived;
                    }
 
                    // Request information on variable values
                    if (data[0] == 'V' && data[1] == 'I') {
                        cmnd_request_variableinfo();
                        goto label_CommandReceived;
                    }
                    // Set number of pulses is currently removed
                    if (data[0] == 'N' && data[1] == 'P') {
                        goto label_CommandReceived; 
                    }
                    // Execute code in debug fields 
                    if (data[0] == 'D' && data[1] == 'B') {
                        cmnd_debug_mode();
                        goto label_CommandReceived; 
                    }
                    // Set value on variable that you want to debug 
                    // (is done in void cmnd_set_debugvariable(char *data) 
                    // in uplink.c )
                    if (data[0] == 'D' && data[1] == 'S') {
                        cmnd_set_debugvariable(data);
                        goto label_CommandReceived;
                    }
                }
            }
        }
    }

label_CommandReceived:
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    if (U1STAbits.PERR == 1) {
        U1STAbits.PERR = 0;
    }
    Nop();

}*/