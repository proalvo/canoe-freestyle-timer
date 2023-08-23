/*

Timer for Freestyle Canoe
Author: Kari Nykänen k_nykanen@hotmail.com

Version: 1.6, output to serial port has been updated
20/10/2020 Version: 1.7, start the timer if "S"tart is received from the serial port 
02/04/2020 Version: 1.8, added support for the rotary switch to change the competition time 
15/05/2022 Version: 1.9, added option to interrupt the run byt pressins start button during 3 seconds
14/08/2023 Version: 1.10, fixed bug with interrupton of the run. Horn continued playing if time was 10-11s and the run was interrupted. It was also possible that horn did not play 10 s warning during next run.

Complies with rules for surface boats:
ICF Canoe Freestyle Competition Rules 2023
https://www.canoeicf.com/sites/default/files/2023_competition_rules_canoe_freestyle.pdf 

5.1.3.a - Where possible, a countdown clock, visible to the athlete, will display
the time remaining for each run.

5.2.5 - The run is audibly finished by an audible warning signal. Ten (10)
seconds prior to the run finishing a different or shorter audible warning signal
informs the athlete of the time remaining.


Arduino Uno digital pin connections:

Pin 2 ---- |------ 10kohm --- GND   (Brown)
           |
           ------> Brown (XLR 3)  switch  (White (XLR 2) <--- +5V   

Pin 3 ---- (+) end buzzer (Velleman VM411) (-)--------- GND  

Pin 4 ---- (+ Red (XLR 1) ) start buzzer (Velleman VM411) (- Green (XLR 4)) ------ GND  

Pin 5 --- (not in use)

Led Matrix Display:
Pin 6 --> LMD pin 2 (A)
Pin 7 --> LMD pin 4 (B)
Pin 8 --> LMD pin 10 (SCLK)
Pin 9 --> LMD pin 1 (OE)
Pin 11 --> LMD pin 12 (DATA)
Pin GND --> LMD pin 3,5,7,9,11,13,15 (GND)

ROTARY SWITCH
Pin A0 ---> rotary switch, common pin
5V --> Rotary swith pin 4 --|1.2Kohm|--pin3---|1.2Kohm|--pin2--|1.2Kohm|--pin1--|330ohm|--GND

 */

#include <SPI.h>
#include <DMD2.h> // library from https://github.com/freetronics/DMD2/
#include <fonts/Timer32.h>
#include <fonts/Timerx32x32.h>


#define DISPLAYS_ACROSS 1  // number of DMD panels sideways
#define DISPLAYS_DOWN 2    // number of DMD panels vertically

SoftDMD dmd(DISPLAYS_ACROSS,DISPLAYS_DOWN);  // DMD controls the entire display

//global variables available anywhere in the program

unsigned long startMillis;    // value of millis() at the time of start
unsigned long currentMillis;  // value of millis() at the moment
unsigned long timeElapsed;    // time elapsed since start

long buttonTimer = 0;
long longPressTime = 100; // time needed to press the start button, 1000 = 1 second, 100 = 100 ms

unsigned long elapsedTime = 0;  // obsolete?
unsigned long pastTime = 0;

unsigned long RUN_TIME = 45000;         // rule 5.1.4, time of competition run in milliseconds - default value
unsigned long WARNING_TIME = 35000;     // rule 5.1.5, time of warning sound in milliseconds
const unsigned int TONE_LENGTH = 1000;

boolean playTone = false; 
boolean playToneOK = false; 
boolean playStartBuzzer = false;
boolean buttonActive = false;

const int buttonPin = 2;          // pushbutton pin 
const int buzzerPin = 3;          // end buzzer pin 3
const int buzzerStartPin = 4;     // start buzzer pin 4

int switchValue = 0;  // rotary switch, v1.8

int buttonState = 0; 

// variables for different modes of operation

const int MODE_READY  = 0;   // ready to start
const int MODE_RUN    = 1;     // running the competition
const int MODE_END_1  = 2;   // play the end tone 
const int MODE_END_2  = 3;   // 
const int MODE_RESET  = 4;   // run was interrupted byt time keeper

int mode;  

char serialCharTime[4]; 
String serialStringTime;
boolean x = false;

unsigned long resetTime = 0; // timer to reset the time during the run
boolean resetActive = false;

int incomingByte = 0; // ver 1.7

/* ***********************************************************************************************
// routine to setup the competition time
**************************************************************************************************/


void selectTime()
{
  if (mode == MODE_READY) { // allow change of time with rotary switch only if mode is MODE_READY i.e. competition is not running

    switchValue = analogRead(A0);  
    if ( switchValue > 900 ) {
      RUN_TIME = 45000;         
      WARNING_TIME = 35000;
    } 

    else if ( switchValue < 900 && switchValue > 550 ) {
      RUN_TIME = 60000;         
      WARNING_TIME = 50000;
    }

    else if ( switchValue < 550 && switchValue > 200 ) {
      RUN_TIME = 45000;         
      WARNING_TIME = 35000;
    }

    else if ( switchValue < 550 && switchValue > 200 ) {
      RUN_TIME = 45000;         
      WARNING_TIME = 35000;
    } 

    else {
      RUN_TIME = 45000;         
      WARNING_TIME = 35000;
    }
             
  }
  return;
}


/* ***********************************************************************************************
// the setup routine runs once when you start or reset the Arduino 
**************************************************************************************************/
void setup() {
  
  Serial.begin(38400);  //start Serial in case we need to print debugging info
  Serial.write("OK\n");  // send OK to serial port
  
  // dmd.setBrightness(255);
  dmd.selectFont(Timer32);
  dmd.begin();
  
  pinMode(buzzerPin, OUTPUT);       // initialize the buzzer pin as an output
  pinMode(buzzerStartPin, OUTPUT);  // initialize the start buzzer pin as an output
  pinMode(buttonPin, INPUT);        // initialize the pushbutton pin as an input

  // setup for the analog input / rotary swith
  pinMode(A0, INPUT);

  mode = MODE_READY;
   
  currentMillis = RUN_TIME;         // 

  // dmd.drawString(1,0,String(RUN_TIME/1000));  // draw start  time to the LED screen

  delay(1000);  
  Serial.write("  \n");  // send empty string to serial port, used to clean the "OK"

  selectTime();
  
  dmd.drawString(1,0,String(RUN_TIME/1000));
}




/* ***********************************************************************************************
// the loop routine runs over and over again forever:
**************************************************************************************************/

void loop() {

   /* ***********************************************************************************************
   // routine to read the switch
   **************************************************************************************************/
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
  }
  
  if (digitalRead(buttonPin) == HIGH  || incomingByte == 83) { // #buttonPressed if START button has been pressed or S received from serial port ...
    incomingByte = 0;

      /* ***********************************************************************************************
      // routine to start the competition if the switch has been pressed
      **************************************************************************************************/ 
      if (mode == MODE_READY || mode == MODE_END_2 ) {  // if mode is READY or Xs are blingking in the end of run

        startMillis = millis();//-longPressTime; // start time is when key was pressed ( current time - when key was pressed
        elapsedTime = 0;      // obsolete?                
        mode = MODE_RUN;                      // set mode to RUN = start the run
        serialStringTime = String(RUN_TIME/1000-elapsedTime)+"\n";  // convert time to be suitable for serial port
        serialStringTime.toCharArray(serialCharTime,4);
        Serial.write(serialCharTime);

            
        dmd.selectFont(Timer32);              // set font
        digitalWrite(buzzerStartPin,HIGH);    // play sound to start the run 
        playStartBuzzer = true;
      } 
      else if (mode == MODE_RUN && resetActive==false) {
        if (millis() - startMillis > 3000) {
          resetTime=millis();
          resetActive = true;
        
        }
      } 
      else if (mode == MODE_RUN && resetActive==true) {
        if ( millis() - resetTime > 3000 ) {
          // resetActive = false;  
          mode = MODE_RESET;
          digitalWrite(buzzerPin,LOW); // make sure that buzzer is silenced, version 1.10
          playTone = false; // version 1.10
          playToneOK = false; // version 1.10
        }
      }
      

  } else if ( resetActive == true ) {
    resetActive=false;
  } 

  currentMillis = millis();  // set current time in millis

  // if competition is running and 45 secods has elapsed, play tones and finish the competition
  
  if ( mode == MODE_RUN ){    // #mode_1 if mode is RUN then check if time has elapsed
    
    if ( currentMillis-startMillis >= RUN_TIME ) { // #end_of_run // if time has elpased then finish the time

      dmd.drawString(1,0,String(RUN_TIME/1000));   // draw the RUN_TIME 
      mode = MODE_END_1;                          // set MODE_END_1 to play tones 
      playTone = false;
      playToneOK = false;

    } // end if  #end_of_run

    // update the display if competition is running
  
    if ( currentMillis-startMillis < RUN_TIME ) { // #runtime

         elapsedTime =  int( (currentMillis - startMillis)/1000 );

         if (elapsedTime != pastTime ) {  // if second has changed then we have to update display also
            
            if ( (RUN_TIME/1000-elapsedTime) <10 ) {  // if time is less than 10 seconds..
              dmd.clearScreen();
              dmd.drawString(9,0,String(RUN_TIME/1000-elapsedTime)); // then write number to the middle of screen
            }
            else {  
              dmd.clearScreen();
              dmd.drawString(1,0,String(RUN_TIME/1000-elapsedTime));   // write number from left to right              
            }
            Serial.println(RUN_TIME/1000-elapsedTime);
            pastTime = elapsedTime;            
         }

        if ( (currentMillis-startMillis) >= TONE_LENGTH && playStartBuzzer == true) {  // if it is TONE_LENGTH from start
          digitalWrite(buzzerStartPin,LOW);               // then silence the start buzzer
          playStartBuzzer = false;
        }

         if ( currentMillis-startMillis >= WARNING_TIME ) { // #warningtones

            if ( playTone == false) {
              digitalWrite(buzzerPin,HIGH);    
              playTone = true;
             }
            
            if ( (currentMillis-startMillis) >= (WARNING_TIME + TONE_LENGTH) && playToneOK == false ) {
              digitalWrite(buzzerPin,LOW);
              playToneOK = true;
            }

         } // end if #warningtones 
         
    }  // end if #runtime 
    
  } // end if #mode_1   

// play end tones, which is two signals, 
// beep (1 sec) - silence (1 sec) - beeb (1 sec)
// total time 3 seconds and you cannot interrupt it by pressing the start switch

  if ( mode == MODE_END_1 ) {
    digitalWrite(buzzerPin,HIGH);  // start playing the tone
    Serial.write("0\n"); 
    dmd.clearScreen();
    dmd.drawString(9,0,String(0));  // show "0" on screen
    delay(1000); 
    digitalWrite(buzzerPin,LOW);
    delay(1000);  // pause
    digitalWrite(buzzerPin,HIGH);
    delay(1000); 
    digitalWrite(buzzerPin,LOW);  
    Serial.write("  \n"); //  send empty string to serial port, used to clean the Green Screen 
    mode = MODE_END_2;
  }

// show flashing "X" in the nd of run, on for 1 second, off for 1 second, this can be interrupted any time by pressing start button 

  if ( mode == MODE_END_2 ) { // #mode_end_2
    dmd.selectFont(Timerx32x32);
    if ( (currentMillis-startMillis) > (RUN_TIME + 3000) && (currentMillis-startMillis) < ( RUN_TIME + 4000) ) { 

      if ( x == false ) {
        dmd.drawString(0,0,"X");
        x = true;
      }

    }

  if ( (currentMillis-startMillis) > (RUN_TIME + 4000) && (currentMillis-startMillis) < ( RUN_TIME + 5000) ) { 
      if ( x == true ) {
        dmd.clearScreen();
        x = false;
      }    
  }

    if ( (currentMillis-startMillis) > (RUN_TIME + 5000) && (currentMillis-startMillis) < ( RUN_TIME + 6000) ) { 

      if ( x == false ) {
        dmd.drawString(0,0,"X");
        x = true;
      }

    }

  if ( (currentMillis-startMillis) > (RUN_TIME + 6000) && (currentMillis-startMillis) < ( RUN_TIME + 7000) ) { 
      if ( x == true ) {
        dmd.clearScreen();
        x = false;
      }    
  }

    if ( (currentMillis-startMillis) > (RUN_TIME + 7000) && (currentMillis-startMillis) < ( RUN_TIME + 8000) ) { 

      if ( x == false ) {
        dmd.drawString(0,0,"X");
        x = true;
      }

    }

  if ( (currentMillis-startMillis) > (RUN_TIME + 8000) && (currentMillis-startMillis) < ( RUN_TIME + 9000) ) { 
      if ( x == true ) {
        dmd.clearScreen();
        x = false;
      }    
  }

    if ( (currentMillis-startMillis) > (RUN_TIME + 9000) && (currentMillis-startMillis) < ( RUN_TIME + 10000) ) { 

      if ( x == false ) {
        dmd.drawString(0,0,"X");
        x = true;
      }

    }

  if ( (currentMillis-startMillis) > (RUN_TIME + 10000) && (currentMillis-startMillis) < ( RUN_TIME + 11000) ) { 
      if ( x == true ) {
        dmd.clearScreen();
        x = false;
      }    
  }


 if ( (currentMillis-startMillis) > (RUN_TIME + 11000) && (currentMillis-startMillis) < ( RUN_TIME + 12000) ) { 

    if ( x == false ) {
      dmd.drawString(0,0,"X");
      x = true;
    }
  }

  if ( (currentMillis-startMillis) > (RUN_TIME + 12000) && (currentMillis-startMillis) < ( RUN_TIME + 13000) ) { 
      if ( x == true ) {
        dmd.clearScreen();
        x = false;
      }    
  }

  if ( (currentMillis-startMillis) > (RUN_TIME + 13000) ) {
    x = false;
    dmd.clearScreen();
    dmd.selectFont(Timer32);
    dmd.drawString(1,0,String(RUN_TIME/1000));
    mode = MODE_READY;
  }

  } // #mode_end_2
    if ( mode == MODE_RESET ){
      Serial.write("  \n");
      dmd.clearScreen();
      dmd.selectFont(Timerx32x32);
      digitalWrite(buzzerStartPin,HIGH);
      dmd.drawString(0,0,"X");
      delay(1000);
      digitalWrite(buzzerStartPin,LOW);
      delay(2000);
      dmd.clearScreen();
      dmd.selectFont(Timer32);
      dmd.drawString(1,0,String(RUN_TIME/1000));
      mode = MODE_READY;
    }
}  // the end
