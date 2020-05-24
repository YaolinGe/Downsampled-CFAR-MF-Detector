#ifndef MAX5419_h
#define MAX5419_h


#include "Arduino.h"
#include <Wire.h>

class MAX5419
{
public:
    // address
    explicit MAX5419(const uint8_t);
	
    uint8_t write(const uint8_t); //write volatile
	uint8_t writeNonvolatile(const uint8_t);  //write to nonvolatile
	uint8_t switchVtoNV(); //switch memory from vol to nonvol
	uint8_t switchNVtoV(); //opposite of above
	
private:
    uint8_t send(const uint8_t, const uint8_t);     // cmd value
    uint8_t _address;
};

#endif