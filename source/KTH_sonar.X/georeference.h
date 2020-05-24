/* 
 * File:   georeference.h
 * Author: viktor
 *
 * Created on April 13, 2018, 11:25 AM
 */

#ifndef GEOREFERENCE_H
#define	GEOREFERENCE_H

#ifdef	__cplusplus
extern "C" {
#endif
void operateGPS(void);
int PPStoCrystalCal(void);
void offlineTimeSet(void);
void start_GPS(void);
void stop_GPS(void);

double get_timeMultiplicator(void);
char* get_datestringSet(void);
char* get_timestringSet(void);
char* get_locationstring(void);

int get_GPSact(void);
int get_GPSfix(void);
char get_GPScyclic(void);

void set_GPSfeed(int gf);
void set_GPSact(int ga);
void set_GPScyclic(int cyc);

#ifdef	__cplusplus
}
#endif

#endif	/* GEOREFERENCE_H */

