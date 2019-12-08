#ifndef POSE_H
#define POSE_H
#include "MotorCommands.h"

//pin definitions
#define WIRE_SENSOR_RIGHT 36
#define WIRE_SENSOR_LEFT 39
#define BTN_FWD_1 4
#define BTN_FWD_2 16
#define BTN_FWD_3 17
#define BTN_BWD 5 

//app contants
#define FWD_SPEED_BASE_1X 2000.0
#define FWD_SPEED_BASE_2X 2800.0
#define FWD_SPEED_BASE_3X 3200.0
#define BWD_SPEED_BASE -2000.0
#define NUM_ADC_SAMPLES 10
#define MAX_SPEED 4000.0
#define MIN_SPEED -4000.0
#define MAX_SENSOR 775
#define SENSOR_CLOSE 150
#define SLOW_SPEED 900.0
#define DEFAULT_GAME_TIME_SEC 600

#define P_GAIN 1.0
#define D_GAIN 0.1

class Payload{
  public:
    Payload();
    void update();    
    int normal_speed;
    int medium_speed;
    int max_speed;
    int bwd_speed;
    int fwd_btn_1;
    int fwd_btn_2;
    int fwd_btn_3;
    int bwd_btn_1;
    int left_sensor;
    int right_sensor;
    float lastError;
    int getNumberForwardButtonsPressed();
    void enable();
    void disable();
    MotorCommand lastCommand; 
    boolean manualDrive;   
    boolean enabled;

  private:
    int readButton(int pin);
    int computeNominalSpeed();
    int readADCPinPeak(int pin,int num_samples);
    int num_fwd_pressed();
    void setVelocity(float left, float right);
    void setBothVelocity(float both);
    

};

class Game{
  public:
    Game();
    int getSecondsRemaining();
    int durationSeconds;
    boolean isOver();
    void start();
  private:
    long startTime;    
};

#endif
