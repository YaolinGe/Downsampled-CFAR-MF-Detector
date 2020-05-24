

#include <Arduino.h>
#include <BridgeCom.h>

#define ECHOTIME 50
#define ACKTIME 200

//----------------------------------------------------------------
BridgeCom::BridgeCom(Stream &stream, uint8_t _myISB_ID, uint8_t pin){   // Constructor
  myISBID = _myISB_ID;                          // My unique ISB address
  RS485DE = pin;
  pBusStream = &stream;                         // Pointer to the Stream for the bus
}
//----------------------------------------------------------------
uint8_t BridgeCom::calc_checksum(char* buffer, uint8_t len) {
  uint8_t chk = 0;
  for (int ii = 0; ii < len; ii++) { chk = chk ^ buffer[ii]; } // XOR
  return chk;
}
//----------------------------------------------------------------
//--------------------send package stuff--------------------------
//----------------------------------------------------------------
void BridgeCom::new_package(uint8_t _toISB, uint8_t  _msgID) {
  send_ptr = send_buffer;
  //Header
  add_byte('#');      //Start byte
  add_byte(_toISB);   //receiver
  add_byte(myISBID);  //Sender
  add_byte(_msgID);   //Message ID
  //DATA 0 - 240 bytes (249?)
  //add_byte(length); // Length of entire message # to CS
  //add_byte('*');
  //add_byte(CS);
}
//----------------------------------------------------------------
bool BridgeCom::send_package() {
  clear_package();                              //Clear incoming buffer

  uint8_t len = (send_ptr - send_buffer) + 3;   //Calcualte length
  add_byte(len);                                //Add length

  //Add Checksum
  add_byte('*');        //Add '*'
  uint8_t cs = calc_checksum(send_buffer,len-1);
  add_byte(cs);         //Add CS

  bool broadcast = send_buffer[1]==255;         //sending to address 255 is boradcast
  bool success = send_package422(!broadcast);

  //reset send buffer;
  send_ptr = send_buffer;

  return success;
}
//----------------------------------------------------------------
bool BridgeCom::send_package422(bool waitForAck) {
  clear_buffer();                               //clear buffer just in case

  //Send the data
  uint8_t send = 0;
  uint16_t len = (send_ptr - send_buffer);
  uint32_t sendingStart_us = micros();
  digitalWriteFast(RS485DE,HIGH);
  for(uint8_t i=0;i<len;i++) {
    pBusStream->write(send_buffer[i]);
  }
  pBusStream->flush();
  digitalWriteFast(RS485DE,LOW);


  //Wait for ack
  if(waitForAck) {
    uint32_t before = micros();
    while(micros()-before < ACKTIME) {
      if(pBusStream->available()) {
        char c = pBusStream->read();
        if(c==0xFF) {continue; }                //was a problem before. might be fixed now
        if(c == 0x06){return true;}             //ACK received
        else {return false;}                    //Received something wrong
      }
    }
  }
  return false;                                 //Timeout
}
//----------------------------------------------------------------
void BridgeCom::clear_buffer() {
  while(pBusStream->available()) {
    while(pBusStream->available()) pBusStream->read();
    delayMicroseconds(20);
  }
}
//----------------------------------------------------------------
//---------------------received package stuff---------------------
//----------------------------------------------------------------
bool BridgeCom::check_4_data() {
if(!pBusStream->available()) return false;      //No data to read
  char c = pBusStream->read();
  receive_buffer.put(c);
  package_available = false;                    //New data added. so the old package has been overwrittern

  if(waitForCS) {if(parse_package()) {waitForCS=false; return true;}} // c was CS. parse package
  if(c == 0x2A) {waitForCS = true;}             // next byte is the Checksum (probably)
  else waitForCS = false;
  return false;
}
//----------------------------------------------------------------
bool BridgeCom::parse_package() {
  uint8_t CS = receive_buffer.get(0);           // newest byte is CS
  uint8_t star = receive_buffer.get(1);         // 2:nd newest byte is '*'
  uint8_t length = receive_buffer.get(2);       // 3:nd newest byte is length
  uint8_t start = receive_buffer.get(length-1); // start byte. Should be '#'

  //Error checks
  if(start != '#'){return false; }
  uint8_t checksum = 0;
  for (int ii = length-1; ii >0 ; ii--) { checksum = checksum ^ receive_buffer.get(ii); } // XOR
  if(checksum != CS) { 
    return false; 
  };

  //package received
  uint8_t receiver = receive_buffer.get(length-2); // Message destination

  //send ack if message was for me
  if(receiver == myISBID){
    digitalWriteFast(RS485DE,HIGH);
    delayMicroseconds(2);
    pBusStream->write(0x06);
    pBusStream->flush();
    delayMicroseconds(2);
    digitalWriteFast(RS485DE,LOW);
  }

  //filter message
  if((receiver != myISBID) && (receiver!=255)) {return false;}  //Message is not for me.

  unpack_index = length-5;
  package_available = true;
  toISB  = receiver;
  fromISBID = receive_buffer.get(length-3);
  msgID = receive_buffer.get(length-4);
  return true;
}
//----------------------------------------------------------------
uint8_t BridgeCom::wait_4_package(int timeout_ms) {
  uint32_t before = millis();
  while(millis() - before < timeout_ms) {
    if(package_available) return msgID;
    check_4_data();
  }
  return 255;
}
//----------------------------------------------------------------
void BridgeCom::clear_package() {
  package_available = false;
  fromISBID = 255;
  toISB = 255;
  msgID = 255;
}
//----------------------------------------------------------------
//-----------------------Add data to package----------------------
//----------------------------------------------------------------
void BridgeCom::add_byte(uint8_t b) {
  if(send_ptr - send_buffer < 255) {
    *send_ptr = b;
    send_ptr++;
  }
}
//----------------------------------------------------------------
void BridgeCom::add_float(float val){
  // Concatinates a float to package (4 bytes)
  myUnionFloat.myFloat = val;
  add_byte(myUnionFloat.bytes[0]);
  add_byte(myUnionFloat.bytes[1]);
  add_byte(myUnionFloat.bytes[2]);
  add_byte(myUnionFloat.bytes[3]);
}
//----------------------------------------------------------------
void BridgeCom::add_long(long val){
  // Concatinates a Long int to package (4 bytes)
  myUnionLong.mylong    = val;
  add_byte(myUnionLong.bytes[0]);
  add_byte(myUnionLong.bytes[1]);
  add_byte(myUnionLong.bytes[2]);
  add_byte(myUnionLong.bytes[3]);
}
//----------------------------------------------------------------
void BridgeCom::add_llong(long long val) {
  // Concatinates a Long long int to package (8 bytes)
  add_long((uint32_t) ((uint64_t) val >> 32) );
  add_long((uint32_t) val);
}
//----------------------------------------------------------------
void BridgeCom::add_int(int val){
   // Concatinates a int to package (2 bytes)
  myUnionInt.myInt = val;
  add_byte(myUnionInt.bytes[0]);
  add_byte(myUnionInt.bytes[1]);
}


//----------------------------------------------------------------
//-----------------------get data from package--------------------
//----------------------------------------------------------------
byte BridgeCom::parse_byte() {
  byte b = receive_buffer.get(unpack_index);
  if(unpack_index > 0) unpack_index--;
  return b;
}
//----------------------------------------------------------------
String  BridgeCom::parse_string(int Nchars){
  String s = "";
  for(int i=0;i<Nchars;i++) s+=(char) parse_byte();
  return s;
};          //
//----------------------------------------------------------------
float BridgeCom::parse_float(){
  // Converts 4 bytes to a float
  myUnionFloat.bytes[0] = parse_byte();
  myUnionFloat.bytes[1] = parse_byte();
  myUnionFloat.bytes[2] = parse_byte();
  myUnionFloat.bytes[3] = parse_byte();
  return myUnionFloat.myFloat;
};
//----------------------------------------------------------------
long  BridgeCom::parse_long(){
  // Converts 4 bytes to a long integer (signed)
  myUnionLong.bytes[0] = parse_byte();
  myUnionLong.bytes[1] = parse_byte();
  myUnionLong.bytes[2] = parse_byte();
  myUnionLong.bytes[3] = parse_byte();
  return myUnionLong.mylong;
};
//----------------------------------------------------------------
long long BridgeCom::parse_llong() {
  //converts 8 bytes to long long integer (signed)
  uint32_t msb = parse_long();
  uint32_t lsb = parse_long();
  uint64_t val = (((uint64_t) msb) << 32 ) +  lsb;
  return val;
}
//----------------------------------------------------------------
int BridgeCom::parse_int(){
  // Converts 2 bytes to an int integer (signed)
  myUnionInt.bytes[0] = parse_byte();
  myUnionInt.bytes[1] = parse_byte();
  return myUnionInt.myInt;
}
