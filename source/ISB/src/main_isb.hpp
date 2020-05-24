
#ifndef MAIN_ISB_H
#define	MAIN_ISB_H

#ifdef	__cplusplus
extern "C" {
#endif

// OPERATION MODES
// MIRROR_MODE: ISB mirrors everything between Serial (usb) and Serial5 (sonar serial)
// Natural order of priority:
#define MIRROR_MODE 0
#define TEST_MODE 1
#define FULL_DATA_STORAGE_MODE 0

#define SONAR0_ID "000"
#define BLINK_PIN 13
#define MAXLEN_SONAR_MSG 300
#define MAXLEN_CAPTAIN_MSG 100
#define TRUE 1
#define FALSE 0
#define DATA_BUFFER_LEN 100000 // For downloading data values

// Timeout handler ----------
#define TIMEOUT 10000 // [us]

// Function prototypes
void mirror_communication(void);
void simulated_captain_input(void);
void real_captain_input(void);

void ledON(void);
void ledOFF(void);

char* get_sonar_message(void);
char* get_captain_message(void);
char* get_data_buffer(void);

#ifdef	__cplusplus
}
#endif
#endif	/* MAIN_ISB_H */