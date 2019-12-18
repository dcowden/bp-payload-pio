#include <Arduino.h>
#include "Payload.h"
#include "MotorCommands.h"

Payload::Payload(){
    normal_speed = FWD_SPEED_BASE_1X;
    medium_speed = FWD_SPEED_BASE_2X;
    max_speed = FWD_SPEED_BASE_3X;
    bwd_speed = BWD_SPEED_BASE;
    fwd_btn_1=0;
    fwd_btn_2=0;
    fwd_btn_3=0;
    bwd_btn_1=0;
    left_sensor=0;
    right_sensor = 0;  
    lastError=0.0;
    manualDrive=false;
    enabled=false;
    MotorCommand mc { 0.0, 0.0, false};
    lastCommand = mc;

}
int Payload::getNumberForwardButtonsPressed(){
  return num_fwd_pressed ();
}
void Payload::enable(){
  enabled = true;
  lastCommand.enabled = true;
}

void Payload::disable(){
  enabled = false;
  setBothVelocity(0.0);
  lastCommand.enabled = false;     
}
void Payload::update(){

  if ( !enabled ){
    return;
  }

  fwd_btn_1 = readButton(BTN_FWD_1);
  fwd_btn_2 = readButton(BTN_FWD_2);
  fwd_btn_3 = readButton(BTN_FWD_3);
  bwd_btn_1 = readButton(BTN_BWD);
  left_sensor = readADCPinPeak(WIRE_SENSOR_LEFT,NUM_ADC_SAMPLES);
  right_sensor= readADCPinPeak(WIRE_SENSOR_RIGHT,NUM_ADC_SAMPLES);

  int nominalSpeed = computeNominalSpeed();
  boolean shouldDrive = ( nominalSpeed != 0 );
  boolean goingForward = ( nominalSpeed > 0 );
  if ( manualDrive){
    if ( fwd_btn_1 ){
      setVelocity(FWD_SPEED_BASE_1X,0.0);
    }
    else if ( fwd_btn_2){
      setVelocity(0.0,FWD_SPEED_BASE_1X);
    }
    else if ( fwd_btn_3 ){
      setBothVelocity(FWD_SPEED_BASE_1X);
    }
    else{
      setBothVelocity(0.0);
    }
    lastCommand.enabled = true;   
    return;
  }

  if ( shouldDrive ){
    int error = left_sensor - right_sensor;
    
    int d_err = error - lastError;
    totalError += error;

    int correction = (float)error * P_GAIN + (float)d_err * D_GAIN  + (float)totalError * I_GAIN;
    lastError = error;

    if (goingForward ){
      if ( left_sensor > SENSOR_CLOSE || right_sensor > SENSOR_CLOSE ){
        nominalSpeed = SLOW_SPEED;
      }
      setVelocity(  constrain(nominalSpeed - correction,MIN_SPEED,MAX_SPEED),
                    constrain(nominalSpeed + correction,MIN_SPEED,MAX_SPEED)
      );
    }
    else{
      
      setBothVelocity(0.0);  
      lastCommand.enabled = false;    
      /**
      if ( left_sensor > SENSOR_CLOSE || right_sensor > SENSOR_CLOSE ){
        nominalSpeed = -SLOW_SPEED;
      }
      setVelocity(  constrain(nominalSpeed - correction,MIN_SPEED,MAX_SPEED),
                    constrain(nominalSpeed + correction,MIN_SPEED,MAX_SPEED)
      ); **/
    }
    lastCommand.enabled = true;
  }
  else{
    lastCommand.enabled = false;
    setBothVelocity(0.0);
  }


  return;
}

int Payload::readButton(int buttonPin){
  int i = digitalRead(buttonPin);
  return i == 0;
}

int Payload::computeNominalSpeed(){
  int num_fwd = num_fwd_pressed();
  if ( num_fwd == 3 ){
    return max_speed ;
  }  
  else if ( num_fwd == 2 ){
    return medium_speed;
  }
  else if ( num_fwd == 1 ){
    return normal_speed;
  }
  else if ( num_fwd == -1 ){
    return bwd_speed;
  }
  else{
    return 0;
  }
}

void Payload::setVelocity(float left, float right){
      lastCommand.leftVelocity = left;
      lastCommand.rightVelocity = right;
}
void Payload::setBothVelocity(float both){
  setVelocity(both,both);
}
int Payload::num_fwd_pressed ( ){
  return fwd_btn_1 + fwd_btn_2 + fwd_btn_3 - bwd_btn_1;
}

int Payload::readADCPinPeak(int pin, int samples ){
  int maxv = 0;
  int v = 0;
  for (int i=0;i<samples;i++){
    v = analogRead(pin);
    if ( v > maxv ) maxv = v;
  }
  return maxv;
}

Game::Game(){
} 

int Game::getSecondsRemaining(){
  long endTime = startTime + 1000*durationSeconds;
  return (int)(endTime - millis() ) / 1000;  
}
boolean Game::isOver(){
  return (getSecondsRemaining() <= 0);
  
}
void Game::start(int gameDurationSeconds){
  durationSeconds = gameDurationSeconds;
  startTime = millis();
}
