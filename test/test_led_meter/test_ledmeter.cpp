//#include <Arduino.h>
#include "Meter.h"
#include <unity.h>
//#include <FastLED.h>

#define LED_COUNT 8
//MeterColor leds[LED_COUNT];

MeterRange testRange [1] = {  { 0, 7 } };
Meter simpleMeter = Meter(testRange,1, MeterColor::Blue, MeterColor::Black);

MeterRange reversedTestRange [1] = {  { 7, 0 } };
Meter reversedMeter = Meter(reversedTestRange,1, MeterColor::Blue, MeterColor::Black);

MeterRange doubleTestRange [2] = {  {0, 3}, { 4, 7}  };
Meter twoRangeMeter = Meter(doubleTestRange,2, MeterColor::Blue, MeterColor::Black);

MeterRange mirroredTestRange [2] = {  {0, 3}, { 7, 4}  };
Meter mirroredRangeMeter = Meter(mirroredTestRange,2, MeterColor::Blue, MeterColor::Black);

MeterColor ALL_BLUE[LED_COUNT] = {MeterColor::Blue, MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,
                            MeterColor::Blue, MeterColor::Blue,MeterColor::Blue, MeterColor::Blue };

MeterColor ALL_BLACK[LED_COUNT] = {MeterColor::Black, MeterColor::Black,MeterColor::Black, MeterColor::Black,
                            MeterColor::Black, MeterColor::Black,MeterColor::Black, MeterColor::Black };

// void resetLEDS(){    
//     for ( int i=0;i<LED_COUNT;i++){
//         leds[i] = MeterColor::Black;
//     }
// }

void assert_leds_equal(MeterColor* expected, MeterColor* actual){
    for (int i=0;i<LED_COUNT;i++,expected++,actual++){
        TEST_ASSERT_EQUAL(*expected,*actual);
    }
}

void test_meter_initially_all_black(void) {
    assert_leds_equal(ALL_BLACK,simpleMeter.leds);
    assert_leds_equal(ALL_BLACK,reversedMeter.leds);
    assert_leds_equal(ALL_BLACK,twoRangeMeter.leds);
    assert_leds_equal(ALL_BLACK,mirroredRangeMeter.leds);
}

void test_basic_meter_initial_state(void){
    TEST_ASSERT_EQUAL(100, simpleMeter.getMaxValue()); 
    TEST_ASSERT_EQUAL(0, simpleMeter.getValue());
}

void test_basic_meter_zero(void){
    simpleMeter.setValue(0);
    assert_leds_equal(ALL_BLACK,simpleMeter.leds);    
}
void test_basic_meter_max_value(void){
    simpleMeter.setValue(100);
    assert_leds_equal(ALL_BLUE,simpleMeter.leds);
}

void test_basic_meter_mid_value(void){
    simpleMeter.setValue(50);
    MeterColor expected[LED_COUNT] = {MeterColor::Blue, MeterColor::Blue,MeterColor::Blue,MeterColor::Blue,
                                MeterColor::Black,MeterColor::Black, MeterColor::Black, MeterColor::Black };
    assert_leds_equal(expected,simpleMeter.leds);
}

void test_basic_meter_nearly_full_value_still_isnt_full(void){
    simpleMeter.setValue(95);
    MeterColor expected[LED_COUNT] = {MeterColor::Blue, MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,
                                MeterColor::Blue,MeterColor::Blue,MeterColor::Blue, MeterColor::Black };
    assert_leds_equal(expected,simpleMeter.leds);
}

void test_basic_meter_tiny_value_still_isnt_lit(void){
    simpleMeter.setValue(11);
    assert_leds_equal(ALL_BLACK,simpleMeter.leds);
}

void test_reversed_meter_zero(void){
    reversedMeter.setValue(0);
    assert_leds_equal(ALL_BLACK,reversedMeter.leds);    
}
void test_reversed_meter_max_value(void){
    reversedMeter.setValue(100);
    assert_leds_equal(ALL_BLUE,reversedMeter.leds);
}

void test_reversed_meter_mid_value(void){
    reversedMeter.setValue(50);
    MeterColor expected[LED_COUNT] = { MeterColor::Black,MeterColor::Black, MeterColor::Black, MeterColor::Black, 
                                MeterColor::Blue,MeterColor::Blue,MeterColor::Blue,MeterColor::Blue };
    assert_leds_equal(expected,reversedMeter.leds);
}

void test_reversed_meter_nearly_full_value_still_isnt_full(void){
    reversedMeter.setValue(95);
    MeterColor expected[LED_COUNT] = {MeterColor::Black, MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,
                                MeterColor::Blue, MeterColor::Blue,MeterColor::Blue, MeterColor::Blue };
    assert_leds_equal(expected,reversedMeter.leds);
}

void test_reversed_meter_tiny_value_still_isnt_lit(void){
    reversedMeter.setValue(11);
    assert_leds_equal(ALL_BLACK,reversedMeter.leds);
}

void test_double_meter_zero(void){
    twoRangeMeter.setValue(0);
    assert_leds_equal(ALL_BLACK,twoRangeMeter.leds);    
}

void test_double_meter_max_value(void){
    twoRangeMeter.setValue(100);
    assert_leds_equal(ALL_BLUE,twoRangeMeter.leds);
}

void test_double_meter_mid_value(void){
    twoRangeMeter.setValue(50);
    MeterColor expected[LED_COUNT] = { MeterColor::Blue,MeterColor::Blue, MeterColor::Black, MeterColor::Black,
                                 MeterColor::Blue,MeterColor::Blue, MeterColor::Black, MeterColor::Black };
    assert_leds_equal(expected,twoRangeMeter.leds);
}

void test_double_meter_nearly_full_value_still_isnt_full(void){
    twoRangeMeter.setValue(95);
    MeterColor expected[LED_COUNT] = {MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,MeterColor::Black, 
                                MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,MeterColor::Black };
    assert_leds_equal(expected,twoRangeMeter.leds);
}

void test_double_meter_tiny_value_still_isnt_lit(void){
    twoRangeMeter.setValue(18);
    assert_leds_equal(ALL_BLACK,twoRangeMeter.leds);
}

void test_mirrored_meter_zero(void){
    mirroredRangeMeter.setValue(0);
    assert_leds_equal(ALL_BLACK,mirroredRangeMeter.leds);    
}

void test_mirrored_meter_max_value(void){
    mirroredRangeMeter.setValue(100);
    assert_leds_equal(ALL_BLUE,mirroredRangeMeter.leds);
}

void test_mirrored_meter_mid_value(void){
    mirroredRangeMeter.setValue(50);
    MeterColor expected[LED_COUNT] = { MeterColor::Blue,MeterColor::Blue, MeterColor::Black, MeterColor::Black,
                                 MeterColor::Black,MeterColor::Black, MeterColor::Blue, MeterColor::Blue };
    assert_leds_equal(expected,mirroredRangeMeter.leds);
}

void test_mirrored_meter_nearly_full_value_still_isnt_full(void){
    mirroredRangeMeter.setValue(95);
    MeterColor expected[LED_COUNT] = {MeterColor::Blue,MeterColor::Blue, MeterColor::Blue,MeterColor::Black, 
                                MeterColor::Black,MeterColor::Blue, MeterColor::Blue,MeterColor::Blue };
    assert_leds_equal(expected,mirroredRangeMeter.leds);
}

void test_mirrored_meter_tiny_value_still_isnt_lit(void){
    mirroredRangeMeter.setValue(18);
    assert_leds_equal(ALL_BLACK,mirroredRangeMeter.leds);
}
void run_tests(){
    UNITY_BEGIN();

    //simple meter tests
    RUN_TEST(test_meter_initially_all_black);
    RUN_TEST(test_basic_meter_initial_state);
    RUN_TEST(test_basic_meter_zero);
    RUN_TEST(test_basic_meter_max_value);
    RUN_TEST(test_basic_meter_mid_value);
    RUN_TEST(test_basic_meter_nearly_full_value_still_isnt_full);
    RUN_TEST(test_basic_meter_tiny_value_still_isnt_lit);

    //reversed meter tests
    RUN_TEST(test_reversed_meter_zero);
    RUN_TEST(test_reversed_meter_max_value);
    RUN_TEST(test_reversed_meter_mid_value);
    RUN_TEST(test_reversed_meter_nearly_full_value_still_isnt_full);
    RUN_TEST(test_reversed_meter_tiny_value_still_isnt_lit);

    //tworange meter 
    RUN_TEST(test_double_meter_zero);
    RUN_TEST(test_double_meter_max_value);
    RUN_TEST(test_double_meter_mid_value);
    RUN_TEST(test_double_meter_nearly_full_value_still_isnt_full);
    RUN_TEST(test_double_meter_tiny_value_still_isnt_lit);

    //two range, mirrored meter
    RUN_TEST(test_mirrored_meter_zero);
    RUN_TEST(test_mirrored_meter_max_value);
    RUN_TEST(test_mirrored_meter_mid_value);
    RUN_TEST(test_mirrored_meter_nearly_full_value_still_isnt_full);
    RUN_TEST(test_mirrored_meter_tiny_value_still_isnt_lit);

    UNITY_END();
}

//TODO: i hate this. to run only on local, use test_ignore in platformio.ini
#ifdef ARDUINO
#include <Arduino.h>
    void setup() {
        delay(2000);
        Serial.begin(115200);
        run_tests();
    }
    void loop() {
        delay(500);
    }
#else
    int main(int argc, char **argv){
        run_tests();
        return 0;
    }
#endif
