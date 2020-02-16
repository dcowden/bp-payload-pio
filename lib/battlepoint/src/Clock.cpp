#include <Clock.h>

#ifdef Arduino
#include <Arduino.h>
long ArduinoClock::milliseconds(){
    return millis();
};
#endif
   

long TestClock::milliseconds() {
    return _currentTime;
};
void TestClock::setTime(long currentTime){
    _currentTime = currentTime;
};
void TestClock::addMillis(long milliSeconds){
    _currentTime += milliSeconds;
};
void TestClock::addSeconds(int seconds){
    _currentTime += (seconds*1000);
};
