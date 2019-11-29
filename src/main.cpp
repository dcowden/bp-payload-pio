#include "Arduino.h"
#include <FastLED.h>

#include "LedMeter.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "MotorCommands.h"
#include "I2CAnything.h"
#include "Payload.h"
#include <menu.h>
#include <menuIO/SSD1306AsciiOut.h>
#include <menuIO/serialIO.h>
#include <menuIO/chainStream.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>


#define menuFont System5x7
#define fontW 5
#define fontH 8

//pin definitions
#define ROTARY_ENCODER_A_PIN 14
#define ROTARY_ENCODER_B_PIN 13
#define ROTARY_ENCODER_BUTTON_PIN 25
#define ENC_STEPS 4
#define LED_PIN 27

//app constants
#define NUM_LEDS 13
#define I2C_ADDRESS 0x3C
#define GAME_DURATION_SECS 600
#define DEBUG 1

#define MAX_DEPTH 2

Game game;

CRGB leds[NUM_LEDS];
SSD1306AsciiWire oled;

LedRange payloadRange [1] = {  { 0, 12 } } ; 
LedMeter payloadMeter = LedMeter(leds,payloadRange,1,CRGB::Blue, CRGB::Black);
Payload payload;


int gameDuration = GAME_DURATION_SECS;
int exitMenuOptions = 0; //Forces the menu to exit and cut the copper tape

Menu::encoderIn<ROTARY_ENCODER_A_PIN,ROTARY_ENCODER_B_PIN> encoder;//simple quad encoder driver
Menu::encoderInStream<ROTARY_ENCODER_A_PIN,ROTARY_ENCODER_B_PIN> encStream(encoder,4);// simple quad encoder fake Stream
Menu::serialIn serial(Serial);

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{-ROTARY_ENCODER_BUTTON_PIN,options->getCmdChar(enterCmd)}};//negative pin numbers use internal pull-up, on = low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

MENU_INPUTS(in,&encStream,&encButton,&serial);

// ESP32 timer thanks to: http://www.iotsharing.com/2017/06/how-to-use-interrupt-timer-in-arduino-esp32.html
// and: https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
//hw_timer_t* timer = NULL;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

enum AppModeValues
{
  APP_GAME_RUNNING,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD
};

int ledCtrl = LOW;

Menu::result myLedOn() {
  ledCtrl = HIGH;
  return Menu::proceed;
}
Menu::result myLedOff() {
  ledCtrl = LOW;
  return Menu::proceed;
}

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
       , VALUE("On", HIGH, doNothing, noEvent)
       , VALUE("Off", LOW, doNothing, noEvent)
      );

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
     , FIELD(gameDuration, "GameTime", "%", 0, 1000, 10, 1, doNothing, noEvent, wrapStyle)
     , OP("LED On", myLedOn, enterEvent)
     , OP("LED Off", myLedOff, enterEvent)
     , EXIT("<Back")
    );

#define MAX_DEPTH 2

//define output device
Menu::idx_t serialTops[MAX_DEPTH] = {0};
Menu::serialOut outSerial(Serial, serialTops);

//describing a menu output device without macros
//define at least one panel for menu output

constMEM Menu::panel panels[] MEMMODE = {{0, 0, 128 / fontW, 64 / fontH}};
Menu::navNode* nodes[sizeof(panels) / sizeof(Menu::panel)]; //navNodes to store navigation status
Menu::panelsList pList(panels, nodes, 1); //a list of panels and nodes
Menu::idx_t tops[MAX_DEPTH] = {0, 0}; //store cursor positions for each level
SSD1306AsciiOut outOLED(&oled, tops, pList, 8, 1+((fontH-1)>>3) ); //oled output device menu driver
Menu::menuOut* constMEM outputs[] MEMMODE = {&outOLED, &outSerial}; //list of output devices
Menu::outputsList out(outputs, sizeof(outputs) / sizeof(Menu::menuOut*)); //outputs list

//MENU_OUTPUTS(menuOut,MAX_DEPTH
//  ,LCD_OUT(outOLED,{0,0,128,64})
//  ,NONE
//);

//macro to create navigation control root object (nav) using mainMenu

NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);


//nav.showTitle = true; // SHow titles in the menus and submenus
//  nav.timeOut = 60;  // Timeout after 60 seconds of inactivity and return to the sensor read screen
//  nav.idleOn(); // Start with the main screen and not the menu


void setupLEDs(){
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  payloadMeter.setMaxValue(NUM_LEDS);  
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

void setup() {
  Serial.begin(115200);
  Wire.begin();
  setupLEDs();
  setupOLED();
}

void startGame() {
    game.start(GAME_DURATION_SECS);
    payloadMeter.setMaxValue(GAME_DURATION_SECS);
    payloadMeter.setToMax();
}

void updateLEDs(){
  payloadMeter.setValue ( game.getSecondsRemaining() );
  payloadMeter.update();
  FastLED.show();
}

void gameOverDisplay(){
  for (int i=0;i<20;i++){
    payloadMeter.setToMax();
    FastLED.show();
    FastLED.delay(100);
    payloadMeter.setToMin();
    FastLED.show();
    FastLED.delay(100);    
  }
  FastLED.delay(5000);
}

void updateDisplay(){  
  char SPACE = ' ';
  oled.setCursor(0,1);
  oled.print("Time Rem: ");
  oled.print(game.getSecondsRemaining());
  oled.print(" s");
  oled.clearToEOL();
  
  oled.setCursor(0,2);
  oled.print("BTNS:");
  oled.print(SPACE);
  oled.print(payload.fwd_btn_1);
  oled.print(SPACE);
  oled.print(payload.fwd_btn_2);
  oled.print(SPACE);
  oled.print(payload.fwd_btn_3);
  oled.print(SPACE);
  oled.print(payload.bwd_btn_1);
  oled.clearToEOL();
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

void loop() {
  payload.update();
  handleCommand(payload.lastCommand);
  
  #if DEBUG
  updateSerial();
  #endif
  
  updateDisplay();
  updateLEDs();

  if ( game.getSecondsRemaining() < 0){
    gameOverDisplay();
  }
  switch (exitMenuOptions) {
    case 1: {
        delay(500); // Pause to allow the button to come up
        //runCuts(); 
        break;
      }
    default: // Do the normal program functions with ArduinoMenu
      nav.poll(); // Poll the input devices
  }
  FastLED.delay(100);  
}