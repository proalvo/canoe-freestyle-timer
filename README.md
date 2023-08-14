# Timer for Canoe Freestyle 
This is the timer software for Canoe Freestyle competitions. The timer complies with International Canoe Federation's (ICF) Canoe Freestyle Competition Rules 2023 for K1, C1, OC1. Squirt is not supported.

Operation is very simple:
- Press a button to start the timer which is running from 45 seconds down to 0. Time is shown on LED dislay. Piezo buzzer provides audible signal that timer is running. 
- Timing can be interrupted by pressing the button 3 seconds. 
- One tone is played 10 seconds before end. 
- Two tones are played in the end.
- Blinking "X" is played when the time has finished.
- The timer is ready for the new run immediately after finishing the time.


See the video on YouTube https://youtu.be/Ev1N3gt9AcE

Software is running on Arduino Uno, which is connected to LED display, loud speaker (including tone genretator and amplifier) and switch. The switch is used to start the timer. 
Time is outputted to serial port of the Arduino Uno, which enables addtional functionality outside of the Timer. 

Software uses DMD2 library, which provides functionality write text and numbers to LED Screen. This library is available from https://github.freetronics/DMD2. I have made own fonts that are more suitable for the timer display.

## Author
Kari Nykänen, k_nykanen (at) hotmail.com

## License

This software is licensed under GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007. Read the LICENSE file.

## Comliance with ICF rules

ICF Rules 2023: https://www.canoeicf.com/sites/default/files/2023_competition_rules_canoe_freestyle.pdf

Following rules apply:

*5.1.3.a - Where possible, a countdown clock, visible to the athlete, will display
the time remaining for each run.*

*5.2.5 - The run is audibly finished by an audible warning signal. Ten (10)
seconds prior to the run finishing a different or shorter audible warning signal
informs the athlete of the time remaining.*

## Hardware
System has the following components:
- Adrduino Uno microcontroller
- 2 x 32x16 LED matrix display, type HUB12
- Handheld switch for the start button, with 10 kohm resistor and piezo buzzer connected through Velleman VMA411 Mos Driving Module (see www.velleman.eu).
- Signal generator, amplifier, and horn loud speaker, connected through Velleman VMA411 Mos Driving Module (see www.velleman.eu) and relay.
- Power supply (12 VDC battery) and voltage stabilizers. I have 24 Ah battery which more than enough for one day.

## Installation

Note: This software is useless without required hardware components. However, you may download the software for learning and development purposes even if you do not have the hardware.

1. Install Arduino IDE from https://arduino.cc to to your computer. 
1. Run Arduino IDE and install DMD2 library from https://github.com/freetronics/DMD2/
1. Copy canoe-freestyle-timer.ino to your computer
1. Copy files from "fonts" directory to "libraries\DMD2\fonts".
1. Connect Arduino UNO to your computer
1. Open the file canoe-freestyle-timer.ino with Arduino INO
1. Upload the code to your Arduino Uno
1. Connect the hardware components.

Now you are ready. Just press the start button.

## Time for streaming video

Arduino Uno has USB port which can transfer competition time to external device. I made Raspberry Pi based device to provide time for streaming video. See detailes at https://github.com/proalvo/green-screen

