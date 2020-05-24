
#ifndef KTH_SONAR_CMD_H
#define	KTH_SONAR_CMD_H

#ifdef	__cplusplus
extern "C" {
#endif

void process_captain_cmd(int cmd_code);
void cmd_measure_depth(void);
void cmd_send_square_pulse(void);
void cmd_download_correlation_vector(void);
void cmd_download_time_vector(void);
void cmd_geosynch(void);

#ifdef	__cplusplus
}
#endif
#endif	/* KTH_SONAR_CMD_H */