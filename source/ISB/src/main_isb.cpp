//** ISBapp example *//
//** PLATFORM IO    *//

#include <ISBteensy.h>
#include "SPI.h"
#include <BridgeCom.h>
#include <msyTime.h>
#include "messages.hpp"
#include "captain_sonarisb_interface.hpp"
#include "main_isb.hpp"
#include "KTH_Sonar_cmd.hpp"

//////////////////////////////  LED blink  /////////////////////////
IntervalTimer myTimer;
IntervalTimer myTimer2;
uint32_t blink_time = 0;

// Buffers
char sonar_message[MAXLEN_SONAR_MSG];
char captain_message[MAXLEN_CAPTAIN_MSG];
char data_buffer[DATA_BUFFER_LEN];

char* get_sonar_message(void){
  return sonar_message;
}
char* get_captain_message(void){
  return captain_message;
}
char* get_data_buffer(void){
  return data_buffer;
}

//----------------------------------------------------------------//

//ISB id
static uint8_t ID = 10; // Adress of the ISB (from point of view of captain)

//ISB library
ISBteensy isb;

//Captain bus
BridgeCom ISBBus(Serial1,ID,3);  //Serial, Id, driver enable

uint32_t t;
//uint16_t cmd_code;

// Main program loop
void loop() {

  if(MIRROR_MODE==TRUE){
    mirror_communication();
   }
  else if(MIRROR_MODE==FALSE){
    // Simulate input from captain
    if(TEST_MODE==TRUE){
      simulated_captain_input();
    }
    else if(TEST_MODE==FALSE){
      real_captain_input();
    }

  }

}

void simulated_captain_input(void){
  uint16_t nread=0;
  if(Serial.available()!=0){
    // Command has been received 
    nread=0;
    // Read from internal buffer until empty
    while(Serial.available()!=0){
      // Read to isb buffer
      if(nread<MAXLEN_CAPTAIN_MSG){
        captain_message[nread]=Serial.read();
        nread++;
      }
    }
    uint16_t cmd_code=atoi(captain_message);
    process_captain_cmd(cmd_code);

  }

}


void real_captain_input(void){

}



void mirror_communication(void){
  uint16_t nread=0;
  // Mirror mode: send command from Serial to sonar
  if(Serial.available()!=0){
    // Reset message character counter
    nread=0;
    // Read from internal buffer until empty
    while(Serial.available()!=0){
      // Read to isb buffer
      if(nread<MAXLEN_CAPTAIN_MSG){
        captain_message[nread]=Serial.read();
        nread++;
      }

    }
    // Repeat message:
    Serial.write("Message echo: \n");
    //Serial.write((int)1); Serial.print(1);Serial.print("\n");
    for(int i=0; i<nread;i++){
      // Print the message
      Serial.write(captain_message[i]);
    }
    Serial.write("\n");
    // Do stuff
    // Pass command to sonar
    for(int i=0; i<nread;i++){
      // Print the message
      Serial5.write(captain_message[i]);
    }

  }

  // Incoming message from sonar, mirror to usb serial
  if(Serial5.available()!=0){
    // Reset message character counter
    nread=0;
    while(Serial5.available()!=0){
      // Read to buffer
      if(nread<MAXLEN_SONAR_MSG){
        sonar_message[nread]=Serial5.read();
        nread++;
      }

    }
    for(int i=0; i<nread;i++){
      // Print the message
      Serial.write(sonar_message[i]);
    }

  }

}

/*
  //print stuff
  if(millis() - t > 5000) {
    Serial.println("ISB::ISB is alive ....barely... with life support");
    t = millis();
  }*/


/*
 * This function must be as fast as possible. 
 * Don't do any computations or sensor updates in this function.
 * Just send the data.
 * 
 * NOTE:
 * This called by an interrupt function and can be called at any time. 
 * Take special care of variables used here and elsewhere in the program 
*/
void msgReceived() {
  switch (ISBBus.msgID) {
    
    case 1:
    // Do something or respond
    // if respond, use 
      //reply with data

      // Max 200 bytes in response
      ISBBus.new_package(0,2); //0=address of captain, 2=msg ID
      //ISBBus.add_byte(0);
      ISBBus.add_float(0);
      //ISBBus.add_int(0);
      //ISBBus.add_long(0);
      //ISBBus.add_llong(0);
      ISBBus.send_package();
    break;
  }
  ISBBus.clear_package();
}


void ledOFF() {
  digitalWriteFast(BLINK_PIN,LOW);
  myTimer2.end();
  uint32_t now = msyTime::microseconds();
  uint32_t test1 = now % 100000;
  myTimer.begin(ledON, 100000-test1);
  myTimer.priority(10);
}

void ledON() {
  digitalWriteFast(BLINK_PIN, HIGH);
  myTimer2.begin(ledOFF,10000);
  myTimer.end();
}


//setup function. Called once
void setup() {

  // Set slightly higher cpu speed
  //CPU_PRESCALE(CPU_8MHz); // 3.3 volt compatible

  // Setup USB serial for debugging
  Serial.begin(9600); // Is always 12mbit

  // Setup Captain serial line
  Serial1.begin(3000000);
  pinMode(3,OUTPUT); digitalWrite(3,LOW); //Driver enable
  pinMode(4,OUTPUT); digitalWrite(4,LOW); //Receive enable


  // Setup serial communication with sonar
  Serial5.begin(921600);

  //isb.setup(false); // No watchdog
  //isb.setup(true); // watchdog enabled
  
  //while(!Serial); //wait untill serial is open

  pinMode(BLINK_PIN,OUTPUT); digitalWrite(BLINK_PIN,LOW);
    
  //start the debug blinking
  ledOFF();
}