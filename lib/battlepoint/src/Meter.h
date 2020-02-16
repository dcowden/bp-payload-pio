#ifndef __INC_METER_H
#define __INC_METER_H

struct MeterRange {
    //zero based
    int startIndex;
    int endIndex;
};

typedef enum {
  Black = 0,
  Red = 1,
  Blue = 2,
  Yellow = 3,
  Green = 4
} MeterColor;


class Meter {
  public:
    Meter (MeterRange* ranges, int new_ranges_cnt,MeterColor new_fgcolor, MeterColor new_bgcolor );    
    void setValue(int value );
    void setMaxValue(int value);
    void setToMax();
    void setToMin();
    int getValue();
    int getMaxValue();
    void setColors(MeterColor fg, MeterColor bg);
    void update();
    MeterColor* leds;
    int num_leds;

  private:
    int value;
    int maxValue;
    void setFgColor(MeterColor color);    
    void setBgColor(MeterColor color);      
    MeterRange* ranges;
    int ranges_cnt;    
    MeterColor fgColor;
    MeterColor bgColor;   
    void updateRange(MeterRange* range);    
};
#endif
