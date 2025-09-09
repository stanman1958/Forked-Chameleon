// August 11, 2025

/* Forked Chameleon Firmware

Copyright 2025 Stan Peterson
Derived from 3D Chameleon MK4.1 Firmware & Hardware

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated 
documentation files (the “Software”), to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions 
of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.

Single Button Press Commands (count pulses of selector)

#1 - 1
#2 - 2
#3 - 3
#4 - 4
#5 - Cut and unload current filament
#9 - Cut filament, mainly for troubleshooting and failed prints
 
*/

#include <SSD1306Ascii.h> //i2C OLED
#include <SSD1306AsciiWire.h> //i2C OLED
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>




#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_I2C_ADDRESS 0x3C
SSD1306AsciiWire oled;

// Updates for CNC Sheild V4, X Motor Driver
#define extEnable 8

#define AChameleonStep 2
#define AChameleonDir 5

// Y Motor Drive
#define BChameleonStep 3
#define BChameleonDir 6


//#define RESET 13


#define trigger A3
//#define s_limit A4
//#define filament A5

const int stepsPerRev = 200;
const int microSteps = 16;
const int speedDelay = 240;

Servo filamentCutter;  // create servo object to control a servo
bool reverseServo = true;

Servo Selector1;
Servo Selector2;

Servo AChameleon;
Servo BChameleon;

int SelectorHome = 90;
int SelectorMin = 15;
int SelectorMax = 165;
int AChameleonOffset = -10;
int BChameleonOffset = 10;
int pos = 0;

int currentFilament = -1;
int lastFilament = -1;

long pulseTime = 200;


//int address = 0;
//byte value;

//long idleCount = 0;
//bool logoActive = false;
//bool T0Loaded = false;
//bool T1Loaded = false;
//bool T2Loaded = false;
//ool T3Loaded = false;

int load = 1;
int unload = 0;



//bool displayEnabled = false;
//bool ioEnabled = false;
//int sensorEnabled = 0;

//long randomNumber = 0;

void setup()
{

  Wire.begin(); //start i2C  
	Wire.setClock(400000L); //set clock

  // enable OLED display
  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS);

  // wait for it to start up
  delay(50);
    

  // welcome screen
  oled.setFont(Adafruit5x7);
  oled.clear(); //clear display
	oled.println("");
  oled.println("Welcome!"); //print a welcome message  
  oled.println("");
  oled.println("Forked Chameleon"); //print a welcome message
  oled.println("Version 12 Sep 6 2025");
  oled.println("Stan Peterson");
  delay(5000);

  Serial.begin(9600);

  


  // Sets the pins as Outputs
  pinMode(extEnable, OUTPUT);
  pinMode(AChameleonStep, OUTPUT);
  pinMode(AChameleonDir, OUTPUT);

  pinMode(BChameleonStep, OUTPUT);
  pinMode(BChameleonDir, OUTPUT);

 // pinMode(RESET, OUTPUT);
 // digitalWrite(RESET, HIGH);

  // set up the button
  pinMode(trigger, INPUT_PULLUP);  // selector button


// comment out the selector cycling code

// Cycle and Home the Selectors - streamline this after debugging
    Selector1.attach(9);
//Selector1.write(SelectorHome);
    for (pos = SelectorHome+AChameleonOffset; pos <= (SelectorMax+AChameleonOffset); pos += 5) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Selector1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
    delay(1000);
    for (pos = (SelectorMax+AChameleonOffset); pos >= (SelectorMin+AChameleonOffset); pos -= 5) { // goes from 180 degrees to 0 degrees
    Selector1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
    }
    delay(1000);
    for (pos = (SelectorMin+AChameleonOffset); pos <= SelectorHome+AChameleonOffset; pos += 5) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Selector1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);  
  }
    delay(1000);
    Selector1.detach();

  Selector2.attach(10);
//  Selector2.write(SelectorHome);
    for (pos = SelectorHome+BChameleonOffset; pos <= SelectorMax+BChameleonOffset; pos += 5) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Selector2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
    delay(1000);
    for (pos = SelectorMax+BChameleonOffset; pos >= SelectorMin+BChameleonOffset; pos -= 5) { // goes from 180 degrees to 0 degrees
    Selector2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
    }
    delay(1000);
    for (pos = SelectorMin+BChameleonOffset; pos <= SelectorHome+BChameleonOffset; pos += 5) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Selector2.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);  
  }
  delay(1000);
  Selector2.detach();
  cutFilament(); // Cut any filament left in the toolhead, on start up, gives the user a chance to reposition the filament at the loading point
}

void loop()
{
  unsigned long nextPulse;
  unsigned long pulseCount = 0;
  // process button press
  if (digitalRead(trigger) != 0)
  {

    displayText(30, "Waiting for Trigger");}

    // keep counting until button is released
    while (digitalRead(trigger) == 0)
    {
      if(pulseCount<pulseTime)
      {
        pulseCount++;
        displayCommand(pulseCount);
      }
      delay(1000);  // each pulse is 1000 milliseconds apart 
    }
    processCommand(pulseCount); // ok... execute whatever command was caught (by pulse count)
    pulseCount = 0;
  // each loop adds 50ms delay, so that gets added AFTER the command is processed before the next one can start
  delay(50);
}

// display command for each button pulse
void displayCommand(long commandCount)
{
  switch(commandCount)
  {
  case 1:
    displayText(5, "Load Filament 1");
    break;
  case 2:
    displayText(5, "Load Filament 2");
    break;
  case 3:
    displayText(5, "Load Filament 3");
    break;
  case 4:
    displayText(5, "Load Filament 4");
    break;
  case 5:
    displayText(5, "Cut & Unload Current Filament");
    break;
  case 9:
    displayText(5, "Cut Filament");
    break;
  default:
    displayText(30, "     No Command");
    break;
  }
}

// execute the pulse count command
void processCommand(long commandCount)
{
  // select case for commands
  switch (commandCount)
  {
  case 1: // unload last loaded Filament (if there is one), load requested filament
    currentFilament = 1;
    displayText(5, "Filament 1 Selected");
    unloadFilament(lastFilament);
    loadFilament(currentFilament);
    displayText(5, "Filament 1 Ready");
    lastFilament = currentFilament;  //update the lastFilament to it's new value of the currentFilament
    break;

  case 2: // unload current, switch to #1, load
    displayText(5, "Filament 2 Selected");
    currentFilament = 2;
    unloadFilament(lastFilament);
    loadFilament(currentFilament);
    displayText(5, "Filament 2 Ready");
    lastFilament = currentFilament;  //update the lastFilament to it's new value of the currentFilament
    break;

  case 3: // unload current, switch to #3, load
    displayText(5, "Filament 3 Selected");
    currentFilament = 3;
    unloadFilament(lastFilament);
    loadFilament(currentFilament);
    displayText(5, "Filament 3 Ready");
    lastFilament = currentFilament;  //update the lastFilament to it's new value of the currentFilament
    break;

  case 4: // unload current, switch to #4, load
    displayText(5, "Filament 4 Selected");
    currentFilament = 4;
    unloadFilament(lastFilament);
    loadFilament(currentFilament);
    displayText(5, "Filament 4 Ready");
    lastFilament = currentFilament;  //update the lastFilament to it's new value of the currentFilament
    break;

  case 5: // unload current
    displayText(5, "Unload Filament " + String(lastFilament));
    unloadFilament(lastFilament);
    displayText(5, "Filament " + String(lastFilament) + " Unloaded");
    lastFilament = -1;
    currentFilament = -1;
    break;

//  case 11:                         //future add, reset the Forked Chameleon based on button trigger
//   digitalWrite(RESET, LOW);
//    delayMicroseconds(2);
//    digitalWrite(RESET,HIGH);
//    break;
  case 9:  // cut the filament, manual unload
      cutFilament();
      break;

  default:
    break;
  }
}

// just the routine to update the OLED
void displayText(int offset, String str)
{
    oled.clear();
    oled.println("");
    oled.println(str);
    oled.println("");
    Serial.println(str);
}



void loadFilament (int filament)
{
  switch(filament)
  {
    case 1:
      displayText(5, "Loading Filament 1");
      break;
    case 2:
      displayText(5, "Loading Filament 2");
      break;
    case 3:
      displayText(5, "Loading Filament 3");
      break;
    case 4:
      displayText(5, "Loading Filament 4");
      break;
  }
  selectFilament(filament);
  rotateExtruder(filament, load);
  releaseFilament();      
  // everybody remember where we parked!
lastFilament = currentFilament;
}

void unloadFilament (int filament)
{
  // make sure we have a real filament selected
  if(filament>0)
  {
    // if so, we need to cut the filament
//    displayText(30, "     Cutting...");
    cutFilament();

    // ok... then wait for the 2nd button press to unload it
    switch(filament)
    {
      case 1:
        displayText(5, "Unloading Filament 1");
        break;
      case 2:
        displayText(5, "Unloading Filament 2");
        break;
      case 3:
        displayText(5, "Unloading Filament 3");
        break;
      case 4:
        displayText(5, "Unloading Filament 4");
        break;
    } 

    selectFilament(filament);
    rotateExtruder(filament, unload);
    releaseFilament();    
  }
}



void selectFilament(int filament)
{
//      displayText(15,"Select Filament Sub " + String(filament));
//      delay(1000);
  int pos;
  int SelectorHome = 90;
  int SelectorMin = 0;
  int SelectorMax = 180;
  switch(filament){
    case 1:
      Selector1.attach(9);
      Selector1.write(SelectorMin+AChameleonOffset);              // tell servo to go to position in variable 'pos'
      delay(1000);                       // waits 1000ms for the servo to reach the position
      break;

    case 2:
      Selector1.attach(9);
      Selector1.write(SelectorMax+AChameleonOffset);              // tell servo to go to position in variable 'pos'
      delay(1000);                          // waits 1000ms for the servo to reach the position
      break;

    case 3:
      Selector2.attach(10);

      Selector2.write(SelectorMin+BChameleonOffset);              // tell servo to go to position in variable 'pos'
      delay(1000);                       // waits 15ms for the servo to reach the position
      break;

    case 4:
      Selector2.attach(10);
      Selector2.write(SelectorMax+BChameleonOffset);              // tell servo to go to position in variable 'pos'
      delay(1000);      // waits 15ms for the servo to reach the position
      break;
  }
}

void releaseFilament()
{
  //displayText(15,"Release Filament Sub");
//  delay(1000);
  Selector1.attach(9);
  Selector1.write(SelectorHome+AChameleonOffset);
    delay(1000);
  Selector1.detach();

  Selector2.attach(10);
  Selector2.write(SelectorHome+BChameleonOffset);
    delay(1000);
  Selector2.detach();
}

// move the extruder motor in a specific direction for a specific distance (unless it's a "until button is not pressed")
void rotateExtruder(int filament, int action)
{
      //displayText(15,"rotateExtruder Sub");
     // delay(500);
     int High = 1;
     int Low = 0;
    int direction;
    const int fastSpeed = speedDelay/2; // double time 
    const long loadTime = 3300; // 1800 did not load the tool head
    const long unloadTime = 1200; //
    int rotationTime;
    unsigned long startTime;

switch (action){
  case 1:
    rotationTime = loadTime;
    break;
  case 0:
    rotationTime = unloadTime;
    break;
}
// set direction depending upon filament and desired action (load or unload)
switch (filament){
  case 1:
    direction = !action;
    rotationTime = 1*rotationTime;
    break;
  case 2:
    direction = action;
    rotationTime = 1*rotationTime;
    break;
  case 3:
    direction = !action;
    rotationTime = 1.03*rotationTime; //the BChameleon seems to need a little extra seems to
    break;
  case 4:
    direction = action;
    rotationTime = 1.03*rotationTime;
  break;
}


//displayText(0,"Direction = " + String(direction));
//delay(1500);

switch (filament){
  case 1:
  case 2:
  digitalWrite(extEnable, LOW);  // lock the motor
  digitalWrite(AChameleonDir, direction); // Enables the motor to move in a particular direction

      startTime = millis();

      while (millis() - startTime < rotationTime)
    {

      // this is how we pulse the motor to get it to step
      digitalWrite(AChameleonStep, HIGH);
      delayMicroseconds(fastSpeed);
      digitalWrite(AChameleonStep, LOW);
      delayMicroseconds(fastSpeed);
    
    }
    break;

  case 3:
  case 4:
    digitalWrite(extEnable, LOW);  // lock the motor
    digitalWrite(BChameleonDir, direction); // Enables the motor to move in a particular direction
      startTime = millis();

      while (millis() - startTime < rotationTime)
    {

      // this is how we pulse the motor to get it to step
      digitalWrite(BChameleonStep, HIGH);
      delayMicroseconds(fastSpeed);
      digitalWrite(BChameleonStep, LOW);
      delayMicroseconds(fastSpeed);
    }
    break;
    }
  // ok
  digitalWrite(extEnable, HIGH);
}


// this cycles the Cutter servo between two positions
void cutFilament() {
//      displayText(15,"Inside Cut Filament Sub");
//      delay(500);
  filamentCutter.attach(11);

  if(reverseServo==false)
  {
    openGillotine();
    closeGillotine();
  }
  else
  {
    closeGillotine();
    openGillotine();
  }
  filamentCutter.detach();
}


// cycle servo from 135 and 180
void openGillotine()
{
    for (int pos = 135; pos <= 180; pos += 1) { // goes from 135 degrees to 180 degrees
    // in steps of 1 degree
    filamentCutter.write(pos);              // tell servo to go to position in variable 'pos'
    delayMicroseconds(25000);                       // waits 25ms for the servo to reach the position
  }
  delay(50);                       
}

// reverse cycle servo from 180 back to 135
void closeGillotine()
{
  for (int pos = 180; pos >= 135; pos -= 1) { // goes from 180 degrees to 135 degrees
    filamentCutter.write(pos);              // tell servo to go to position in variable 'pos'
    delayMicroseconds(25000);                       // waits 25ms for the servo to reach the position
  }
  delay(50);                       
}



