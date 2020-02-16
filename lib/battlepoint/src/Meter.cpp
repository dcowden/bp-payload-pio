#include "Meter.h"
#include "util.h"

#define DEFAULT_MAX 100

void Meter::setValue(int new_value){
  value = new_value;
  update();
};
void Meter::setToMax(){
  setValue(maxValue);
};
void Meter::setToMin(){
  setValue(0);
};
void Meter::setMaxValue(int new_max_value){
  maxValue = new_max_value;
};

int Meter::getValue(){
  return value;
};
int Meter::getMaxValue(){
  return maxValue;
};

Meter::Meter( MeterRange* new_ranges, 
        int new_ranges_cnt, MeterColor new_fgcolor, MeterColor new_bgcolor ){
  value=0;
  maxValue = DEFAULT_MAX;          
  ranges_cnt = new_ranges_cnt;
  ranges = new_ranges;
  fgColor = new_fgcolor;
  bgColor = new_bgcolor; 

  int max_index = 0;
  struct MeterRange* ptr = ranges;
  for ( int i=0;i<ranges_cnt;i++,ptr++){
    if ( ptr->endIndex > max_index ) max_index = ptr->endIndex;
    if ( ptr->startIndex > max_index ) max_index = ptr->startIndex;
  }
  num_leds = max_index + 1;
  leds = new MeterColor[num_leds];

};

void Meter::setColors(MeterColor fg, MeterColor bg){
  setFgColor(fg);
  setBgColor(bg);
};

void Meter::setFgColor(MeterColor new_color){
  fgColor = new_color;
};
void Meter::setBgColor(MeterColor new_color){
  bgColor = new_color;
};


void Meter::update(){
  struct MeterRange* ptr = ranges;
  for ( int i=0;i<ranges_cnt;i++,ptr++){
    updateRange(ptr);
  }
};

void Meter::updateRange(MeterRange* range){

  int startIndex = range->startIndex;
  int endIndex = range->endIndex;
  int pixels_on_index = 0;
  if ( endIndex > startIndex){
    pixels_on_index  = map(getValue(),0,getMaxValue(),startIndex-1,endIndex);
    for ( int i=startIndex;i<= endIndex;i++){
      if ( i <= pixels_on_index  ){
        leds[i] =fgColor;
      }
      else{
        leds[i] = bgColor;
      }
    }    
  }
  else{
    pixels_on_index  = map(getValue(),0,getMaxValue(),startIndex+1,endIndex);
    for ( int i=endIndex;i<= startIndex;i++){
      if ( i >= pixels_on_index  ){
        leds[i] =fgColor;
      }
      else{
        leds[i] = bgColor;
      }
    }    
  }

}
