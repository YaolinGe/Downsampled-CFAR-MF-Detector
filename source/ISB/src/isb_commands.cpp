#include <ISBteensy.h>
#include <BridgeCom.h>
#include <msyTime.h>

#ifndef ISB_VERSION
#define ISB_VERSION 2
#endif

extern ISBteensy isb;
extern BridgeCom ISBBus;

void msgReceived();
void captainSerialEvent(Stream*s);

//=======================================================
void printTimeStamp(timestamp ts);
void PrintNumberV2(timestamp ts);
//=======================================================


///////////////////////// software time sync ///////////////////////
struct timeObject{
  timestamp captain_time;
  int16_t send_time;
  timestamp received;
  uint8_t sequence = 255;
};
timeObject syncObjects[2];
uint8_t syncObjectIndex = 0;

//----------------------------------------------------------------//


#if ISB_VERSION == 1
void serialEvent1() {captainSerialEvent(&Serial1);};
#elif ISB_VERSION == 2
void serialEvent3() {captainSerialEvent(&Serial3);};
#elif ISB_VERSION == 3
//Does not exist yet
#endif

/////////////////////////// Teensy ISB V1 //////////////////////////
void captainSerialEvent(Stream *s) {
  int available_bytes = s->available();
  for(int i=0;i<available_bytes && !ISBBus.package_available; i++) {
    ISBBus.check_4_data();
  }

  if (ISBBus.package_available){ // Now check for app-specific commands
    switch (ISBBus.msgID) {
      case 200:
      {
        uint8_t change_time = ISBBus.parse_byte(); //enable time sync
        uint64_t cTime = ISBBus.parse_llong();
        int16_t last_sendTime = ISBBus.parse_int(); //time it took to send last message
        uint8_t sequence = ISBBus.parse_byte();

        syncObjects[(syncObjectIndex+1)%2].captain_time = cTime;
        syncObjects[(syncObjectIndex+1)%2].received = msyTime::getTimeStamp();
        syncObjects[(syncObjectIndex+1)%2].sequence = sequence;
        syncObjects[(syncObjectIndex)%2].send_time = last_sendTime; //time it took to send the last message

        ISBBus.new_package(0,201);
        ISBBus.add_long((long)millis());  // Millis internal counter
        ISBBus.add_float(isb.voltage);    // Volt
        ISBBus.add_float(isb.current);    // Amps
        ISBBus.add_float(isb.energy);     // Energy consumption Joule
        ISBBus.add_float(isb.temp);       // Temperature °C
        ISBBus.add_float(isb.baro_hPa);   // Barometer hPa
        ISBBus.send_package();
       
        if(change_time) {
          //Check if we should sync time
          if(syncObjects[syncObjectIndex].sequence == (sequence -1)) {
            
            //we have received two consecutive timestamp messages -> now we can change the time
            timestamp elapsed = msyTime::getTimeStamp() - syncObjects[syncObjectIndex].received; //OK
            timestamp ts = syncObjects[syncObjectIndex].captain_time;

            ts += elapsed;
            ts += syncObjects[syncObjectIndex].send_time;

            timestamp before = msyTime::getTimeStamp();
            msyTime::setTime(ts);
            timestamp after = msyTime::getTimeStamp();
            int32_t diff = (int32_t) ((int64_t) before-after);
            
            /*
            //Change the speed of time!
            if(diff > 5) {
              noInterrupts();
              SYST_RVR = SYST_RVR + 1;
              interrupts();  
            }
            else if(diff < -5) {
              noInterrupts();
              SYST_RVR = SYST_RVR - 1;
              interrupts();  
            }
            */

            //Data is no longer valid since the time changed. This will cause a bad sequence
            syncObjects[(syncObjectIndex+1)%2].sequence = sequence-5;
            syncObjects[(syncObjectIndex)%2].sequence = sequence-5;
          }
        }
        syncObjectIndex = (syncObjectIndex+1) % 2;
      }
      break;
      case 206:
        CPU_RESTART;
      break;
      default:
        msgReceived();
      break;
    }
    ISBBus.clear_package(); // Important to be ready for new message
  }
}