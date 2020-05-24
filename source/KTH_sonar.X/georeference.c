#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif
#include <string.h>
#include "georeference.h"
#include "downlink.h"

int GPSact = 0, GPSfeed = 0;
int calval;
char gpsdata[500];
char timestring[6];
char timestringSet[6];
char datestring[6];
char datestringSet[6];
char locationstring[50];
char fix = 0;
char cyclic = 0;
double timeMultiplicator = 0.6666666666667;

//Added interrupt handlers;
void __attribute__((interrupt(IPL1SRS), vector(_EXTERNAL_4_VECTOR)))
PPS_GPS(void);

void PPS_GPS(void) {
    printString("_PPS_");
    printEL();
    calval = TMR6;
    TMR6 = 0;
    int ii = 0;
    do {
        timestringSet[ii] = timestring[ii];
        datestringSet[ii] = datestring[ii];
        ii++;
    } while (ii < 6);
    IEC0bits.T6IE = 0;
    IFS0CLR = _IFS0_INT4IF_MASK; // Clear interrupt?
}

void __attribute__((interrupt(IPL2AUTO), vector(_TIMER_7_VECTOR)))
Timer7Handler(void);

void Timer7Handler(void) {
    TMR6 = 0;
    IFS1bits.T7IF = 0;
    offlineTimeSet();
}

void operateGPS(void) {
    int ii = 0;
    if (U5STAbits.OERR == 1) {
        U5STAbits.OERR = 0;
    }
    if (U5STAbits.URXDA != 0) {
        char gpd = U5RXREG;
        if (GPSfeed == 1) {
            // Copy from UART5 to UART1
            U1TXREG = gpd;
        }
        if (gpd == '$') {
            ii = 0;
            if (gpsdata[1] == 'G' && gpsdata[2] == 'N' && gpsdata[3] == 'R' && gpsdata[4] == 'M' && gpsdata[5] == 'C') {
                memset(timestring, 0, sizeof (timestring));
                memset(datestring, 0, sizeof (datestring));
                timestring[0] = gpsdata[7];
                timestring[1] = gpsdata[8];
                timestring[2] = gpsdata[9];
                timestring[3] = gpsdata[10];
                timestring[4] = gpsdata[11];
                timestring[5] = gpsdata[12];
                int ls = strlen(gpsdata) + 1;
                datestring[0] = gpsdata[ls - 16];
                datestring[1] = gpsdata[ls - 15];
                datestring[2] = gpsdata[ls - 14];
                datestring[3] = gpsdata[ls - 13];
                datestring[4] = gpsdata[ls - 12];
                datestring[5] = gpsdata[ls - 11];
            }
            if (gpsdata[1] == 'G' && gpsdata[2] == 'N' && gpsdata[3] == 'G' && gpsdata[4] == 'S' && gpsdata[5] == 'A') {
                fix = 0;
                fix = gpsdata[9] - 0x30;
            }
            if (gpsdata[1] == 'G' && gpsdata[2] == 'N' && gpsdata[3] == 'G' && gpsdata[4] == 'G' && gpsdata[5] == 'A') {
                char *p;
                int posi[20];
                int ii = 0;
                p = strchr(gpsdata, ',');
                while (p != NULL) {
                    posi[ii] = p - gpsdata + 1;
                    p = strchr(p + 1, ',');
                    ii++;
                }
                memcpy(locationstring, &gpsdata[posi[1]], posi[5] - posi[1] - 1);
                locationstring[posi[5] - posi[1] - 1] = '\0';
                Nop();
            }
            memset(gpsdata, 0, sizeof (gpsdata));
        }
        gpsdata[ii] = gpd;
        ii++;
    }
}

//function for calibrating crystal from GPS;

int PPStoCrystalCal(void) {
    long calibval = 0, returnval = 0, timeout = 0;
    int ii = 0;

    do {
        timeout = 0;
        do {
            timeout++;
        } while (TMR6 > 60 && timeout < 160000000);
        calibval += calval;
        ii++;
    } while (ii < 10 && timeout < 159999999);
    calibval = calibval / 10;
    if (calibval < 200000000 && calibval > 100000000 && timeout < 159999999) {
        returnval = calibval;
        PR6 = 2 * calibval;
        timeMultiplicator = 100000000.0 / calibval;
        Nop();
    } else {
        returnval = timeout;
    }
    return returnval;

}


//Added function to set time when GPS is unavailable;

void offlineTimeSet(void) {

    char hourss, minutess, secondss;
    //yearss, monthss, dayss;
    hourss = (timestring[0] - 0x30) * 10;
    hourss = hourss + (timestring[1] - 0x30);
    minutess = (timestring[2] - 0x30) * 10;
    minutess = minutess + (timestring[3] - 0x30);
    secondss = (timestring[4] - 0x30) * 10;
    secondss = secondss + (timestring[5] - 0x30);

    secondss += 2;

    if (secondss >= 60) {
        secondss = secondss - 60;
        minutess++;
        if (minutess == 60) {
            minutess = 0;
            hourss++;
            if (hourss == 24) {
                hourss = 0;
            }
        }
    }
    timestring[0] = hourss / 10;
    timestring[1] = hourss % 10;
    timestring[2] = minutess / 10;
    timestring[3] = minutess % 10;
    timestring[4] = secondss / 10;
    timestring[5] = secondss % 10;

}

void start_GPS(void) {
    // 32 bit mode! Timer6 and Timer7 are combined, timer operations are 
    // performed by Timer6, and interrupt functions by Timer7
    T6CONbits.TON = 0; // Stop Timer6
    T6CONbits.TCS = 0; // Choose internal clock instead of external (default)
    T6CONbits.T32 = 1; // Combine Timer6 and Timer7 into a 32-bit timer)
    T6CONbits.TCKPS = 0b000; // System clock divider 32bit mode   
    PR6 = 0xFFFFFFFF; // Pre-scaler (interrupt when register has counted to the value of PR6)
    IFS1bits.T7IF = 0; // Clear interrupt flag
    IPC8bits.T7IP = 2; // Set priority 2 
    IEC1bits.T7IE = 1; // Enable interrupt 
    T6CONbits.TON = 1; // Activate Timer6/7



    //IEC1bits.T7IE = 1; // Enable interrupt on Timer 6/7

}


void stop_GPS(void) {
    IEC1bits.T7IE = 0; // Disable interrupt 
    IFS1bits.T7IF = 0; // Clear interrupt flag
    T6CONbits.TON = 0; // Stop Timer6/7 
    T6CON = 0; // Clear control registers for timer 6/7
    T7CON = 0;

    //IEC1bits.T7IE = 0; // Enable interrupt on Timer 6/7
}

double get_timeMultiplicator(void) {
    return timeMultiplicator;
}

char* get_datestringSet(void) {
    return datestringSet;
}

char* get_timestringSet(void) {
    return timestringSet;
}

char* get_locationstring(void) {
    return locationstring;
}

int get_GPSact(void) {
    return GPSact;
}

int get_GPSfix(void) {
    return fix;
}

char get_GPScyclic(void) {
    return cyclic;
}

void set_GPSfeed(int gf) {
    GPSfeed = gf;
}

void set_GPSact(int ga) {
    GPSact = ga;
}

void set_GPScyclic(int cyc) {
    cyclic = cyc;
}