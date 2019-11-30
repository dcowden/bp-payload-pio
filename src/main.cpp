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

#define MAX_DEPTH 1
//Menu Includes
#include <menuIO/keyIn.h>
#define menuFont System5x7
#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>

#define fontW 5
#define fontH 8

//pin definitions
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 13
#define ROTARY_ENCODER_BUTTON_PIN 12
#define ENC_STEPS 4
#define LED_PIN 27

//app constants
#define NUM_LEDS 13
#define I2C_ADDRESS 0x3C
#define GAME_DURATION_SECS 30
#define GAME_OVER_DANCE_SECS 3
#define GAME_OVER_DANCE_DELAY_MS 100
#define DEBUG 1

int gameDuration = GAME_DURATION_SECS;

Game game;

CRGB leds[NUM_LEDS];
SSD1306AsciiWire oled;
ESP32Encoder encoder;
LedRange payloadRange [1] = {  { 0, 12 } } ; 
LedMeter payloadMeter = LedMeter(leds,payloadRange,1,CRGB::Blue, CRGB::Black);
Payload payload;
OneButton encButton(ROTARY_ENCODER_BUTTON_PIN, true);

//function prototypes
void updateDisplay();
void updateLEDs();
void updateSerial();
void sendMotorCommand();
void encButton_SingleClick();
void encButton_DoubleClick();

Ticker displayTimer(updateDisplay,500);
Ticker serialTimer(updateSerial,500);
Ticker motorCommandTimer(sendMotorCommand,10);

enum AppModeValues
{
  APP_GAME_RUNNING,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD
};
int appMode = APP_MENU_MODE;

/*  MENU STUFF
//NO input
Menu::chainStream<0> in(NULL);

//define output device

//describing a menu output device without macros
//define at least one panel for menu output

constMEM Menu::panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
Menu::navNode* nodes[sizeof(panels) / sizeof(Menu::panel)]; //navNodes to store navigation status
Menu::panelsList pList(panels, nodes, 1); //a list of panels and nodes
Menu::idx_t tops[MAX_DEPTH] = {0, 0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 1+((fontH-1)>>3) ); //oled output device menu driver
Menu::menuOut* constMEM outputs[] MEMMODE = {&outOLED, &outSerial}; //list of output devices
Menu::outputsList out(outputs, sizeof(outputs) / sizeof(Menu::menuOut*)); //outputs list

//nav.showTitle = true; // SHow titles in the menus and submenus
//  nav.timeOut = 60;  // Timeout after 60 seconds of inactivity and return to the sensor read screen
//  nav.idleOn(); // Start with the main screen and not the menu
*/
//int currentEncoderCount = encoder.getCount();
//int encoderButtonState = 0;
void setupLEDs(){
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
}

void setupOLED(){
  Wire.setClock(400000L);  
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.displayRemap(false);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.println("BattlePoint v1.0"); 
  oled.println("Starting..."); 
}

void handleCommand(MotorCommand mc){
    Wire.beginTransmission (MOTOR_CONTROLLER_ADDRESS );
    I2C_writeAnything (mc);
    Wire.endTransmission ();  
}

void startGame() {
    Serial.print("Game Starting: ");
    Serial.print(gameDuration);
    Serial.println( " secs.");
    game.start(gameDuration);
    payloadMeter.setMaxValue(gameDuration);
    payloadMeter.setToMax();
    appMode = APP_GAME_RUNNING;
}



void setupEncoder(){
  encButton.attachDoubleClick(encButton_DoubleClick);
  encButton.attachClick(encButton_SingleClick);
  encButton.setDebounceTicks(80);
}

void setup() {
  displayTimer.start();
  serialTimer.start();
  motorCommandTimer.start();
  Serial.begin(115200);
  Wire.begin();
  setupLEDs();
  setupOLED();
  setupEncoder();
  Serial.println("Menu 4.x");
  Serial.println("Use keys + - * /");
  Serial.println("to control the menu navigation");
  encoder.attachSingleEdge(ROTARY_ENCODER_A_PIN,ROTARY_ENCODER_B_PIN);
}

void updateLEDs(){
  payloadMeter.setValue ( game.getSecondsRemaining() );
  payloadMeter.update();
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
  oled.setCursor(0,1);
  oled.print("Time Rem: "); oled.print(game.getSecondsRemaining());  oled.print(" s"); oled.clearToEOL();
  oled.setCursor(0,2);
  oled.print("BTNS:");  oled.print(SPACE); oled.print(payload.fwd_btn_1);
  oled.print(SPACE); oled.print(payload.fwd_btn_2); oled.print(SPACE); oled.print(payload.fwd_btn_3);
  oled.print(SPACE); oled.print(payload.bwd_btn_1); oled.clearToEOL();
  oled.setCursor(0,3);
  oled.print("SENS: L:");
  oled.print(payload.left_sensor);
  oled.print(" R:");   
  oled.print(payload.right_sensor);
  oled.print(SPACE);
  oled.clearToEOL();
  oled.setCursor(0,4);
  oled.print("MTR: L ");  
  oled.print(payload.lastCommand.leftVelocity);
  oled.print(" R: ");
  oled.print(payload.lastCommand.rightVelocity);
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
   Serial.println(SPACE);     
}

Menu::result doStartGame() {
  Serial.println("Starting Game...");
  startGame();
  return proceed;
}


MENU(mainMenu, "BattlePoint Payload v0.1", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,FIELD(gameDuration,"GameTime","",0,1000,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(payload.bwd_speed,"BwdSpeed","",-200,0,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(payload.normal_speed,"FwdSpeed1X","",20,200,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(payload.medium_speed,"FwdSpeed2X","",60,220,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
  ,FIELD(payload.max_speed,"FwdSpeed3X","s",80,255,10,1, Menu::doNothing, Menu::noEvent, Menu::wrapStyle)
  ,OP("StartGame",doStartGame,Menu::enterEvent)
  ,EXIT("<Back")
);

Menu::serialIn serial(Serial);
MENU_INPUTS(in,&serial);

MENU_OUTPUTS(out,MAX_DEPTH
  ,SERIAL_OUT(Serial)
  ,NONE//must have 2 items at least
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);


EncoderMenuDriver encoderDriver ( &nav, &encoder, &encButton);
void encButton_SingleClick(){
  encoderDriver.button_clicked();
}
void encButton_DoubleClick(){
  encoderDriver.button_dbl_clicked();
}

void sendMotorCommand(){
    Wire.beginTransmission (MOTOR_CONTROLLER_ADDRESS );
    I2C_writeAnything (payload.lastCommand);
    Wire.endTransmission (); 
}
void loop() {

  if ( appMode == APP_GAME_RUNNING){
    payload.update();

    #if DEBUG
    updateSerial();
    #endif
    displayTimer.update();
    serialTimer.update();
    motorCommandTimer.update();

    //updateDisplay();
    updateLEDs();

    if ( game.isOver() ){
      appMode = APP_MENU_MODE;
      gameOverDisplay();   
      nav.refresh();         
    }
  }
  else if ( appMode == APP_MENU_MODE){      
    encoderDriver.update();
    FastLED.delay(100);    
  }
  
}