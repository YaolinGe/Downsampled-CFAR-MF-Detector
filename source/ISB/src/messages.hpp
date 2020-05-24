/* 
 * File:   messages.h
 * Author: vikto
 *
 * Created on February 28, 2019, 1:59 PM
 */

#ifndef MESSAGES_H
#define	MESSAGES_H

#ifdef	__cplusplus
extern "C" {
#endif

// Message codes
#define MSG_STARTUP 1
#define MSG_STORAGE_ALLOCATION_SUCCESSFUL 2
#define MSG_BUFFER_ALLOCATION_SUCCESSFUL 3
#define MSG_ALLOCATION_UNSUCCESSFUL 4
#define MSG_TIMEOUT_RECEIVE_FILE 5
#define MSG_WAITING_FOR_FILE 6
#define MSG_STORAGE_OVERFLOW 7
#define MSG_CMND_SV 8
#define MSG_CMND_SF 9
#define MSG_CMND_SNP 10
#define MSG_CMND_AP 11
#define MSG_CMND_SQP 12
#define MSG_CMND_SP 13
#define MSG_CMND_PL 14
#define MSG_CMND_THR 15
    
// Printing function
void printMessage(int msgcode); 

#ifdef	__cplusplus
}
#endif

#endif	/* MESSAGES_H */

