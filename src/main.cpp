#include "Arduino.h"
#include <FastLED.h>
#include "LedMeter.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "MotorCommands.h"
#include "I2CAnything.h"
#include "Payload.h"

#include "OneButton.h"
#include <ESP32Encoder.h>
#include "EncoderMenuDriver.h"
#include "Ticker.h"

#define MAX_DEPTH 2
//Menu Includes
#include <menuIO/keyIn.h>
#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

//#define menuFont System5x7
//#define fontW 5
//#define fontH 7

#define menuFont X11fixed7x14
#define fontW 7
#define fontH 14

//pin definitions
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 13
#define ROTARY_ENCODER_BUTTON_PIN 12
#define ENC_STEPS 4
#define LED_PIN 27

//app constants
#define NUM_LEDS 60
#define I2C_ADDRESS 0x3C
#define GAME_DURATION_SECS 30
#define GAME_OVER_DANCE_SECS 5
#define GAME_OVER_DANCE_DELAY_MS 100
#define DISPLAY_UPDATE_INTERVAL_MS 200
#define MOTOR_UPDATE_INTERVAL_MS 50
#define DEBUG 1

Game game;

CRGB leds[NUM_LEDS];
SSD1306AsciiWire oled;
ESP32Encoder encoder;
LedRange payloadRange [2] = {  { 1, 29 } , { 58,30 }} ; //
LedMeter payloadMeter = LedMeter(leds,payloadRange,2,CRGB::Blue, CRGB::Black);
Payload payload;
OneButton encButton(ROTARY_ENCODER_BUTTON_PIN, true);

//function prototypes
void updateDisplay();
void updateLEDs();
void updateSerial();
void sendMotorCommand(){
    Wire.beginTransmission (MOTOR_CONTROLLER_ADDRESS );
    I2C_writeAnything (payload.lastCommand.leftVelocity);
    I2C_writeAnything (payload.lastCommand.rightVelocity);
    I2C_writeAnything (payload.lastCommand.enabled);
    Wire.endTransmission (); 
}
void encButton_SingleClick();
void encButton_DoubleClick();

Ticker displayTimer(updateDisplay,DISPLAY_UPDATE_INTERVAL_MS,0,MILLIS);
Ticker serialTimer(updateSerial,DISPLAY_UPDATE_INTERVAL_MS,0,MILLIS);
Ticker motorCommandTimer(sendMotorCommand,MOTOR_UPDATE_INTERVAL_MS,0,MILLIS);

enum AppModeValues
{
  APP_GAME_RUNNING,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD,
  APP_MANUAL_DRIVE_MODE
};
int appMode = APP_MENU_MODE;


void setupLEDs(){
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
}

void setupOLED(){
  Wire.setClock(400000L);  
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(menuFont);
  oled.displayRemap(true);
  oled.clear();
  oled.println("BattlePoint v1.0"); 
  oled.println("Starting..."); 
}

void startGame() {
    Serial.print("Game Starting: ");
    Serial.print(game.durationSeconds);
    Serial.println( " secs.");
    game.start();
    payloadMeter.setMaxValue(game.durationSeconds);
    payloadMeter.setToMax();
    oled.clear();
    payload.enable();
    appMode = APP_GAME_RUNNING;
}

void setupEncoder(){
  encButton.attachDoubleClick(encButton_DoubleClick);
  encButton.attachClick(encButton_SingleClick);
  encButton.setDebounceTicks(80);
  encoder.attachHalfQuad(ROTARY_ENCODER_A_PIN,ROTARY_ENCODER_B_PIN);
}
void setupButtons(){
  pinMode(BTN_FWD_1,INPUT_PULLUP);
  pinMode(BTN_FWD_2,INPUT_PULLUP);
  pinMode(BTN_FWD_3,INPUT_PULLUP);
  pinMode(BTN_BWD,INPUT_PULLUP);
}
void setupSensors(){
  pinMode(WIRE_SENSOR_RIGHT,INPUT_PULLUP);
  pinMode(WIRE_SENSOR_LEFT,INPUT_PULLUP);
}
void setup() {
  Serial.begin(57600);
  Wire.begin();
  Wire.setClock(100000);  
  setupLEDs();
  setupOLED();
  setupEncoder();
  setupButtons();
  displayTimer.start();
  serialTimer.start();
  motorCommandTimer.start();
  payload.disable();

}

void updateLEDs(){
  payloadMeter.setValue ( game.getSecondsRemaining() );
  FastLED.show();
}

void gameOverDisplay(){
  
  long end_time = millis() + (GAME_OVER_DANCE_SECS*1000);
  while ( millis() < end_time ){
    payloadMeter.setToMax();
    FastLED.show();
    FastLED.delay(GAME_OVER_DANCE_DELAY_MS);    
    payloadMeter.setToMin();
    FastLED.show();
    FastLED.delay(GAME_OVER_DANCE_DELAY_MS);    
  }
  Serial.println("Game Over!!");
}

void updateDisplay(){  
  char SPACE = ' ';
  oled.setCursor(0,0);
  oled.print("Time Rem: "); 
  oled.print(game.getSecondsRemaining());  
  oled.println(" s"); 
  oled.clearToEOL();
  oled.print("BTNS:");  
  oled.print(SPACE); 
  oled.print(payload.fwd_btn_1);
  oled.print(SPACE); 
  oled.print(payload.fwd_btn_2); 
  oled.print(SPACE); 
  oled.print(payload.fwd_btn_3);
  oled.print(SPACE); 
  oled.println(payload.bwd_btn_1); 
  oled.clearToEOL();
  oled.print("SEN: ");
  oled.print(payload.left_sensor);
  oled.print(" ");   
  oled.println(payload.right_sensor);
  oled.clearToEOL();
  oled.print("MTR: ");  
  oled.print((int)payload.lastCommand.leftVelocity);
  oled.print(" ");
  oled.print((int)payload.lastCommand.rightVelocity);
  oled.clearToEOL();

}

void updateSerial(){
   char SPACE = ' ';
   Serial.print("B:");
   Serial.print(SPACE);
   Serial.print(payload.fwd_btn_1);
   Serial.print(SPACE);
   Serial.print(payload.fwd_btn_2);
   Serial.print(SPACE);
   Serial.print(payload.fwd_btn_3);
   Serial.print(SPACE);
   Serial.print(payload.bwd_btn_1);
   Serial.print(SPACE);
   Serial.print("L:");
   Serial.print(payload.left_sensor);
   Serial.print(SPACE);
   Serial.print("R:");   
   Serial.print(payload.right_sensor);
   Serial.print(SPACE);     
   Serial.print("LM:");
   Serial.print(payload.lastCommand.leftVelocity);
   Serial.print(SPACE);   
   Serial.print("RM:");
   Serial.print(payload.lastCommand.rightVelocity);
   Serial.print(" EN:");
   Serial.print(payload.lastCommand.enabled);
   Serial.print(" Mode:");
   Serial.print(payload.manualDrive);
   Serial.print(" enabled:");
   Serial.println(payload.enabled);
   
}

Menu::result doStartGame() {
  Serial.println("Starting Game...");
  startGame();
  return proceed;
}

Menu::result doStartManualControl(){
  Serial.println("Manual Mode Enabled");
  payload.manualDrive = true;  
  payload.enable();
  return proceed;
}

Menu::result doEndManualControl(){
  Serial.println("Auto Mode");
  payload.disable();
  payload.manualDrive = false;
  return proceed;
}


MENU(settingsSubMenu, "Settings", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
    ,FIELD(game.durationSeconds,"Game Time","",0,1000,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
    ,FIELD(payload.bwd_speed,"BwdSpeed","",-200,0,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
    ,FIELD(payload.normal_speed,"FwdSpeed1X","",20,200,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
    ,FIELD(payload.medium_speed,"FwdSpeed2X","",60,220,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
    ,FIELD(payload.max_speed,"FwdSpeed3X","",80,255,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
    , EXIT("<Back")
);

int driveMode=1;
TOGGLE(driveMode,driveModeMenu,"Motors: ",doNothing,noEvent,wrapStyle
  ,VALUE("MAN",0,doStartManualControl,noEvent)
  ,VALUE("AUTO",1,doEndManualControl,noEvent)
);

MENU(mainMenu, "BP Payload v0.1", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("Start Game!",doStartGame,Menu::enterEvent)
  ,SUBMENU(driveModeMenu)
  ,SUBMENU(settingsSubMenu)
);

Menu::serialIn serial(Serial);

//define output device
Menu::idx_t serialTops[MAX_DEPTH] = {0};
serialOut outSerial(Serial, serialTops);

//describing a menu output device without macros
//define at least one panel for menu output
constMEM Menu::panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
Menu::navNode* nodes[sizeof(panels) / sizeof(Menu::panel)]; //navNodes to store navigation status
Menu::panelsList pList(panels, nodes, 1); //a list of panels and nodes
Menu::idx_t tops[MAX_DEPTH] = {0,0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, fontW, 1+((fontH-1)>>3) ); //oled output device menu driver
menuOut* constMEM outputs[] MEMMODE = {&outOLED, &outSerial}; //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*)); //outputs list


//NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);
NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);

EncoderMenuDriver encoderDriver ( &nav, &encoder, &encButton);
void encButton_SingleClick(){
  encoderDriver.button_clicked();
}
void encButton_DoubleClick(){
  encoderDriver.button_dbl_clicked();
} 
void loop() {
  
  payload.update();
  motorCommandTimer.update();  
  serialTimer.update();
  updateLEDs();

  if ( appMode == APP_GAME_RUNNING){
    //check button, so we can cancel
    encButton.tick();    
    displayTimer.update();
    if ( encButton.isLongPressed() || game.isOver() ){
      appMode = APP_MENU_MODE;
      payload.disable();
      oled.clear();
      oled.println("");
      oled.println("-----------------");
      oled.println("-- GAME OVER   --");
      oled.println("-----------------");
      oled.println("");
      gameOverDisplay(); 
      oled.clear();  
      nav.refresh();         
    }
  }
  else if ( appMode == APP_MENU_MODE){
    encoderDriver.update();          
    FastLED.delay(100);    
  }
  //
}