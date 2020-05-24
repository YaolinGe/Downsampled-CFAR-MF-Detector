#include "KTH_Sonar_cmd.hpp"
#include "captain_sonarisb_interface.hpp"
#include "main_isb.hpp"
#include "SPI.h"

void process_captain_cmd(int cmd_code){

  // Process command -------------
  switch(cmd_code){

    case CPT_MSG_DEPTH:
        break;

    case CPT_MSG_SQP:
        break;

    // Download correlation vector
    case CPT_MSG_DCV:
        cmd_download_correlation_vector();
        break;

        // Download storage
    case CPT_MSG_DTV:
        cmd_download_time_vector();
        break;

    case CPT_MSG_GSY:
        break;

    default:
        break;
  } 

}

void cmd_measure_depth(void){

}

void cmd_send_square_pulse(void){

}

void cmd_download_correlation_vector(void){
    unsigned long t0; // For timeout
    char*data_buffer=get_data_buffer();

    int valflag=0;
    uint16_t nread=0;
    Serial5.write("$");
    Serial5.write(SONAR0_ID);
    Serial5.write("$DCV$"); // This vector needs to be changed
    //Serial5.write("$000$DSV$"); // This vector needs to be changed

    t0=micros();
    // Wait until there are values present
    while(Serial5.available()!=1){
        if(micros()-t0>TIMEOUT){
            break;  
        }
    }
    nread=0;
    // Data is arriving, save it
    while(micros()-t0<TIMEOUT){
    // Values arrive a bit sporadically, force the receiver to  
    // wait out a timeout
    while(Serial5.available()!=0){
        if(nread<DATA_BUFFER_LEN){
        data_buffer[nread]=Serial5.read();
        nread++;
        }
        // Flag that values were loaded
        if(valflag==0){
        valflag=1;
        }
    }
    if(valflag==1){
        // Values were loaded just now, reset timeout
        t0=micros();
        valflag=0;
    }

    }
    Serial.write("Number of values: "); Serial.print(nread); Serial.write("\n");

    // Write to sd card

}

void cmd_download_time_vector(void){
    unsigned long t0; // For timeout
    char*data_buffer=get_data_buffer();

    int valflag=0;
    uint16_t nread=0;
    Serial5.write("$");
    Serial5.write(SONAR0_ID);
    Serial5.write("$DTV$"); // This vector needs to be changed
    //Serial5.write("$000$DSV$"); // This vector needs to be changed

    t0=micros();
    // Wait until there are values present
    while(Serial5.available()!=1){
        if(micros()-t0>TIMEOUT){
            break;  
        }
    }
    nread=0;
    // Data is arriving, save it
    while(micros()-t0<TIMEOUT){
    // Values arrive a bit sporadically, force the receiver to  
    // wait out a timeout
    while(Serial5.available()!=0){
        if(nread<DATA_BUFFER_LEN){
        data_buffer[nread]=Serial5.read();
        nread++;
        }
        // Flag that values were loaded
        if(valflag==0){
        valflag=1;
        }
    }
    if(valflag==1){
        // Values were loaded just now, reset timeout
        t0=micros();
        valflag=0;
    }

    }
    Serial.write("Number of values: "); Serial.print(nread); Serial.write("\n");

    // Write to sd card

}

void cmd_geosynch(void){

}
