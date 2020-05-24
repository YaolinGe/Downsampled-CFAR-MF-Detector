/*sample write sketch
#include <MAX5419.h>
#include <Wire.h>

MAX5419 D1(0x28);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  D1.write(254);
}
*/


#include "MAX5419.h"

#define MAX5419_VREG 		0x11
#define MAX5419_NVREG		0x21
#define MAX5419_NVREGxVREG  0x61
#define MAX5419_VREGxNVREG  0x51


MAX5419::MAX5419(const uint8_t address)
{
	_address = address;
}

uint8_t MAX5419::write(const uint8_t value)  //Standard write command
{
	uint8_t loc = MAX5419_VREG;
    return send(loc, value);
}

uint8_t MAX5419::writeNonvolatile(const uint8_t value) //Write to nonvolatile memory
{
	uint8_t loc = MAX5419_NVREG;
    return send(loc, value);
}

uint8_t MAX5419::switchNVtoV()  //Switch from NV to V
{
	uint8_t loc = MAX5419_NVREGxVREG;
	uint8_t value = 0x00;
    return send(loc, value);
}

uint8_t MAX5419::switchVtoNV() //Switch from V to NV
{
	uint8_t loc = MAX5419_VREGxNVREG;
	uint8_t value = 0x00;
    return send(loc, value);
}


uint8_t MAX5419::send(const uint8_t loc, const uint8_t value)  //send function
{
    Wire.beginTransmission(_address);
    Wire.write(loc);
    Wire.write(value);
    return Wire.endTransmission();
}