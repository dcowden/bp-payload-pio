#include "hardware.h"
#include "Arduino.h"
int sampleADCPin (int pin, int num_samples){
  int maxv = 0;
  long total = 0;
  int v = 0;
  for (int i=0;i<num_samples;i++){
    v = analogRead(pin);
    total += v;
    //if ( v > maxv ) maxv = v;
  }
  return total/num_samples;
}
boolean readPin(int buttonPin){
  int i = digitalRead(buttonPin);
  return i == 0;    
}