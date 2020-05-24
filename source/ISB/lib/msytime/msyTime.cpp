#include "msyTime.h"
#include <Arduino.h>
#include <TimeLib.h>

//static stuff
uint32_t msyTime::lastMicros = 0;
uint32_t msyTime::overrun_counter = 0;
uint32_t msyTime::micros_offset;

timestamp msyTime::getTimeStamp() {
    update();
    timestamp t = (((uint64_t) overrun_counter) << 32) + microseconds();
    return t;
}

uint32_t msyTime::microseconds() {
  return micros() + micros_offset;
}

uint32_t msyTime::milliseconds() {
  return microseconds()/1000;
}

void msyTime::setTime(timestamp newTime) {
    uint32_t offset_before = micros_offset;
    //Serial.println("Set time");
    uint32_t newMicros = (uint32_t) newTime;
    micros_offset = newMicros - micros();
    overrun_counter = newTime >> 32;
    lastMicros = microseconds();
    update();
}

void msyTime::update() {
    uint32_t now = microseconds();
    if(lastMicros > now) {
        overrun_counter +=1;
        //Serial.println("Micros overrun");
        //Serial.print("Overrun_counter: "); Serial.println(overrun_counter);
    }
    lastMicros = now;
}

///////////////////////////////////////////////////////////////////////
uint16_t msyTime::getYear(timestamp &ts) {
    time_t t = ts / 1000000; //seconds since 1970
    return year(t);
}

uint16_t msyTime::getMonth(timestamp &ts) {
    time_t t = ts / 1000000; //seconds since 1970
    return month(t);
}

uint16_t msyTime::getDay(timestamp &ts) {
    time_t t = ts / 1000000; //seconds since 1970
    return day(t);
}

uint16_t msyTime::getHour(timestamp &ts) {
    timestamp temp = ts / 1000000;
    time_t t = (uint32_t) temp; //seconds since 1970
    return hour(t);
}

uint16_t msyTime::getMinute(timestamp &ts) {
    timestamp temp = ts / 1000000;
    time_t t = (uint32_t) temp; //seconds since 1970
    return minute(t);
}

uint16_t msyTime::getSecond(timestamp &ts) {
    timestamp temp = ts / 1000000;
    time_t t = (uint32_t) temp; //seconds since 1970
    return second(t);
}

uint16_t msyTime::getMillis(timestamp &ts) {
    uint32_t ms = (ts / 1000) % 1000;
    return (uint16_t) ms;
}

uint16_t msyTime::getMicros(timestamp &ts) {
    uint32_t us = ts % 1000;
    return (uint16_t) us;
}
