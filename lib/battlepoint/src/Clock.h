#ifndef __INC_CLOCK_H
#define __INC_CLOCK_H

class Clock{
    public:
        virtual long milliseconds() = 0;        
        static int millis_to_seconds(long timeInMillis){
            return (int)timeInMillis / 1000;
        };

        int secondsSince(long startTimeMillis){
            long now = milliseconds();
            return (int)(now - startTimeMillis) / 1000;
        };        
};

#ifdef ARDUINO
#include "Arduino.h"
class ArduinoClock : public Clock {
    public:
        virtual long milliseconds();        
};
#endif

class TestClock : public Clock {
    public:
        TestClock(){
            _currentTime = 0;
        };
        virtual long milliseconds();
        void setTime(long millis);
        void addMillis(long millis);        
        void addSeconds(int seconds);
    private:
        long _currentTime;
};
#endif