#include <Clock.h>
#include <unity.h>


void test_fake_clock(void){
    TestClock tc = TestClock();
    TEST_ASSERT_EQUAL(0, tc.milliseconds());
    tc.setTime(200);
    TEST_ASSERT_EQUAL(200,tc.milliseconds());
    delay(400);
    TEST_ASSERT_EQUAL(200,tc.milliseconds());    

}

void test_fake_clock_add(void){
    TestClock tc = TestClock();
    TEST_ASSERT_EQUAL(0, tc.milliseconds());
    tc.addMillis(200);
    TEST_ASSERT_EQUAL(200,tc.milliseconds());
    tc.addMillis(400);
    TEST_ASSERT_EQUAL(600,tc.milliseconds());        
}

void test_clock_seconds_since(void){
    TestClock tc = TestClock();
    TEST_ASSERT_EQUAL(0, tc.milliseconds());
    tc.addMillis(4000);
    TEST_ASSERT_EQUAL(3,tc.secondsSince(1000));
}
void run_tests(){
    UNITY_BEGIN();
    RUN_TEST(test_fake_clock);
    RUN_TEST(test_fake_clock_add);
    RUN_TEST(test_clock_seconds_since);
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
