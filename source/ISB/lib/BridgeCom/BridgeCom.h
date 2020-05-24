/*------------------------------------------------------------------------------------
	Intelligent Sensor Bridge Communication §rotocol, BridgeCom

	Commands 0-199 user defined commands
  Commands 200-255 system commands

  Jakob Kuttenkeuler
  v 0.5. Jan 2018

  Niklas Rolleberg
  v 0.5.1 Jan 2019
------------------------------------------------------------------------------------*/

#ifndef BridgeCom_h
#define BridgeCom_h

#include "Arduino.h"
#include <Stream.h> // for Stream

#include "CircleBuffer.h"

typedef union { char bytes[4]; long  mylong;  } unionLong;    // Used for conversion
typedef union { char bytes[2]; int   myInt;   } unionInt;     // Used for conversion
typedef union { char bytes[4]; float myFloat; } unionFloat;   // Used for conversion

//----------------------------------------------------------------
class BridgeCom{
public:
  BridgeCom(Stream &stream,uint8_t _isbID, uint8_t pin = 3);        // Constructor
  void          new_package(uint8_t _toISB, uint8_t  _msgID);
  bool          send_package();                    // Sends the package that is prepared
  void          add_byte(uint8_t b);
  void          add_string(String s);              //
  void          add_float(float val);              //
  void          add_long(long val);                //
  void          add_llong(long long val);          //
  void          add_int(int val);                  //

  bool          check_4_data();                    // read one byte from the serial and parse stuff

  byte          parse_byte();
  String        parse_string(int Nchars);          //
  float         parse_float();                     //
  long          parse_long();                      //
  long long     parse_llong();                     //
  int           parse_int();                       //


  void          clear_package();                   // clear incoming package
  uint8_t       wait_4_package(int timeout_ms=100);//

  bool package_available = false;
  uint8_t fromISBID;
  uint8_t toISB;
  uint8_t msgID;


private:
  //Settings
  uint8_t		    myISBID; 						               // The identification number of this ISB, (0=Captain)

  //Hardware constants
  uint8_t       RS485DE;                           // The ISB Driver enable pin
  Stream        *pBusStream;                       // Pointer to Stream for the bus-comm

  uint8_t       calc_checksum(char* buffer, uint8_t len); // Function for calculating XOR checksum

  unionLong     myUnionLong;                       // Help variable for bytes-long conversion
  unionInt      myUnionInt;                        // Help variable for bytes-inte conversion
  unionFloat    myUnionFloat;                      // Help variable for bytes-float conversion

  //buffers
  char send_buffer[255] = {0};                    //Buffer for outgoing data
  char* send_ptr = send_buffer;
  CircleBuffer receive_buffer;                    //Buffer for incoming data
  uint8_t unpack_index;

  //stuff for parsing
  bool waitForCS = false;
  bool parse_package();                           // Check if a package as been received. and call the callback function
  void clear_buffer();                            // Clear incoming data buffer

  //stuff for sending
  bool send_package422(bool waitForAck);          //RS422
};
//----------------------------------------------------------------
#endif
