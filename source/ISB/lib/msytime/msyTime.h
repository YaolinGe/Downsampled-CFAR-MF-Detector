#ifndef MSYTIME_H
#define MSYTIME_H

#include <stdint.h>

typedef uint64_t timestamp;

class msyTime {
private:
    static uint32_t overrun_counter; //Keeps track of how many times micros has been overrun.
    static uint32_t lastMicros;
    static uint32_t micros_offset;
public:
    static timestamp getTimeStamp();
    static uint32_t microseconds();
    static uint32_t milliseconds();

    static void setTime(timestamp newTime);

    static void update();

    static uint16_t getYear(timestamp &t);
    static uint16_t getMonth(timestamp &t);
    static uint16_t getDay(timestamp &t);
    static uint16_t getHour(timestamp &t);
    static uint16_t getMinute(timestamp &t);
    static uint16_t getSecond(timestamp &t);
    static uint16_t getMillis(timestamp &t);
    static uint16_t getMicros(timestamp &t);
};

template<class T>
struct TimestapmedValue {
    T value;
    timestamp ts;
    
    TimestapmedValue() {};
    TimestapmedValue(T _val) {value = _val; ts=msyTime::getTimeStamp();};               //assign
    TimestapmedValue(T _val, timestamp _t) {value = _val; ts = _t;};                    //assign
    TimestapmedValue(const TimestapmedValue<T> &tv) {value = tv.value; ts = tv.ts;};    //copy
    
    T getValue() {return value;};
    void setValue(T _val) {value = _val;};
    
    timestamp getTimestamp() {return ts;};
    void setTimestamp(timestamp _ts) {ts =_ts;};

    template<class T2>
    operator T2() const {return (T2) value;} // This is the conversion operator

    TimestapmedValue<T>& operator+=(const float& rhs) {
        this->value += rhs;
        return *this; // return the result by reference
    }
    
    bool operator!=(const float& rhs) {
        return rhs != this->value;
    }

    bool operator==(const float& rhs) {
        return rhs == this->value;
    }
};

#endif
