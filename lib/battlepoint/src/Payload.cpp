//#include <Arduino.h>
#include "Payload.h"
#include "MotorCommands.h"

Payload::Payload(Clock* _clock){
    lastError=0.0;
    manualDrive=false;
    enabled=false;
    MotorCommand mc { 0.0, 0.0, false};
    lastCommand = mc;
    gameClock = _clock;
}

int Payload::getNumberForwardButtonsPressed(){
  return numFwdPressed ();
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
bool Payload::isDefended(){
  return pose.redButton;
}

void Payload::update(PayloadPose newPose){
  pose = newPose;
  if ( !enabled ){
    return;
  }

  if ( manualDrive){
    computeManualDrive();
  }
  else{
    computeInGameDrive();
  }

}

void Payload::computeManualDrive(){
  if ( pose.bluButton ){
    setVelocity(options.fwdSpeed_1,0.0);
  }
  else if ( pose.btn2){
    setVelocity(0.0,options.fwdSpeed_1);
  }
  else if ( pose.btn3 ){
    setBothVelocity(options.fwdSpeed_1);
  }
  else{
    stopAllMotors();
  }
}

void Payload::computeInGameDrive(){
  int nominalSpeed = computeNominalSpeed();
  bool shouldDrive = ( nominalSpeed != 0 );
  bool goingForward = ( nominalSpeed > 0 );

  if ( shouldDrive ){
    int error = pose.leftWireSensor - pose.rightWireSensor;
    
    int d_err = error - lastError;

    int correction = (float)error * P_GAIN + (float)d_err * D_GAIN ;
    lastError = error;

    if (goingForward ){
      if ( pose.leftWireSensor > SENSOR_CLOSE || pose.rightWireSensor > SENSOR_CLOSE ){
        nominalSpeed = SLOW_SPEED;
      }
      setVelocity(  constrain_value(nominalSpeed - correction,MIN_SPEED,MAX_SPEED),
                    constrain_value(nominalSpeed + correction,MIN_SPEED,MAX_SPEED)
      );
    }
    else{   
      //later we want to actually drive backwards.
      //but that will require more sensors
      stopAllMotors();    
    }
  }
  else{
    stopAllMotors();
  }

}

int Payload::computeNominalSpeed(){
  int num_fwd = numFwdPressed();
  if ( pose.redButton ){
    return 0;
  }
  else{
    if ( num_fwd == 3 ){
      return options.fwdSpeed_3 ;
    }  
    else if ( num_fwd == 2 ){
      return options.fwdSpeed_2;
    }
    else if ( num_fwd == 1 ){
      return options.fwdSpeed_1;
    }
    else{
      return 0;
    }
  }
}

void Payload::setVelocity(float left, float right){
  lastCommand.leftVelocity = left;
  lastCommand.rightVelocity = right;
  lastCommand.enabled = true;
}
void Payload::setBothVelocity(float both){
  setVelocity(both,both);
}

void Payload::stopAllMotors(){
    lastCommand.enabled = false;
    setBothVelocity(0.0);  
}
bool Payload::isContested(){
  return (fwdPressed() &&  pose.bluButton );
}
bool Payload::isMoving(){
  return (fwdPressed()  && (! pose.bluButton));
}
bool Payload::fwdPressed ( ){  
  numFwdPressed() > 0;
}

int Payload::numFwdPressed(){
  int total = 0;
  if ( pose.bluButton) total += 1;
  if ( pose.btn2 ) total += 1;
  if ( pose.btn3) total += 1;
  return total;
}

Game::Game(Payload* _payload){
  payload = _payload;
  overTime = false;
  running =false;
} 

bool Game::isOverTime(){
  return overTime;
}

int Game::getSecondsRemaining(){
  if ( running ){
    long endTime = startTime + 1000*options.gameTimeSeconds;
    return (int)(endTime - gameClock->milliseconds() ) / 1000; 
  }
  else{
    if ( DEBUG ){ Serial.println("WARN: getSecondsRemaining when not running"); };
    return 0;
  }
}

bool Game::isOver(){
  if ( running ){
    if ( shouldEndGame() ){
      if ( DEBUG ){ Serial.println("Ending Game"); };
      running = false;
      overTime = false;
      return true;
    }
    else{
      return false;
    }
  }
  else{
    return true;
  }
}

bool Game::shouldEndGame(){
  if ( getSecondsRemaining() <= 0 ){
    if ( payload->isMoving() ){   
      //attackers ONLY possess        
      lastNonPossessionTime = millis();
      return false;
    }
    else if ( payload->isDefended() ){
      //defenders ONLY possess, or contest
      return true;
    }
    else{
      //nobody has possession 
      int non_posession_time = ( millis() - lastNonPossessionTime);
      if ( non_posession_time > ENDGAME_OVERTIME_MAX_NON_POSSESSION_MILLIS){
        return true;
      }
      else{
        return false;
      }
    }
  }
  else{
    return false;  
  }
}

void Game::start(){
  startTime = millis();
  running = true;
}
