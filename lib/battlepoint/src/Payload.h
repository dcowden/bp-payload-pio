#ifndef PAYLOAD_H
#define PAYLOAD_H
#include "MotorCommands.h"
#include "util.h"
#include "Clock.h"
//#include "pinmap.h"

//app contants

#define MAX_SPEED 6000.0
#define MAX_MENU_SPEED 4000
#define MIN_FORWARD_SPEED 1000.0
#define MIN_SPEED -6000.0
#define SENSOR_CLOSE 2500
#define SENSOR_CENTERED 100
#define SLOW_SPEED 2000.0
#define ENDGAME_OVERTIME_MAX_NON_POSSESSION_MILLIS 5000 //how long can elapse with attackers not possessing

#define P_GAIN 1.0
#define D_GAIN 0.02

#define DEBUG true

typedef struct GameOptions
{
    //stuff in here gets stored to the EEPROM
    int firmwareVersion;  // Firmware version to check if we need to overwrite
    long gameTimeSeconds;        
    long fwdSpeed_1;      
    long fwdSpeed_2;    
    long fwdSpeed_3; 
    long bwdSpeed_1; 
    long rssiThreshold;         
};

typedef struct {
    int leftWireSensor =0;
    int rightWireSensor =0;
    bool bluButton=0;
    bool redButton=0;
    bool btn2=0;
    bool btn3=0;
    int nearAddress=0;
} PayloadPose;


class Payload{
  public:
    Payload(Clock* clock);
    void update(PayloadPose newPose);  
    GameOptions options;
    PayloadPose pose;
    Clock* gameClock;
    float lastError;
    int getNumberForwardButtonsPressed();
    bool isMoving();
    bool isContested();
    bool isDefended();
    void enable();
    void disable();
    MotorCommand lastCommand; 
    bool manualDrive;   
    bool enabled;

  private:
    int readButton(int pin);
    int computeNominalSpeed();
    int readADCPinPeak(int pin,int num_samples);
    bool fwdPressed();
    int numFwdPressed();
    void setVelocity(float left, float right);
    void setBothVelocity(float both);
    void computeManualDrive();
    void computeInGameDrive();
    void stopAllMotors();

};

class Game{
  public:
    Game(Payload* _payload);
    int getSecondsRemaining();
    int durationSeconds;
    bool isOver();
    bool isOverTime();
    void start();
    GameOptions options;
  private:
    long startTime;    
    Payload* payload;
    long lastNonPossessionTime;
    bool overTime;
    bool running;
    bool shouldEndGame();
};

#endif
