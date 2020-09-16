# Timer for Canoe Freestyle 
This is the timer software Canoe Freestyle competitions. The timer complies with ICF Canoe Freestyle Competition Rules 2019. 

Operation is very simple:
- Press a button to start the timer which is running from 45 seconds down to 0. Time is shown on LED dislay. Piezo buzzer provides audible signal that timer is running. 
- One tone is played 10 seconds before end. 
- Two tones are played in the end.
- Blinking "X" is played when the time has finished.
- The timer is ready for the new run immediately after finishing the time.

See the video on YouTube https://youtu.be/Ev1N3gt9AcE

Software is running on Arduino Uno, which is connected to LED display, loud speaker and switch. The switch is used to start the timer. 
Time is outputted to serial port of the Arduino Uno, which enables addtional functionality outside of the Timer. 

Software uses DMD2 library, which provides functionality write text and numbers to LED Screen. This library is available from https://github.freetronics/DMD2. I have made own fonts that are more suitable for the timer display.

# Author
Kari Nykänen, k_nykanen (at) hotmail.com

## License

This software is licensed under GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007. Read the LICENSE file.

## Hardware
System has the following components:
- Adrduino Uno microcontroller
- 2 x 32x16 LED matrix display, type HUB12
- Handheld switch for the start button, with 10 kohm resistor and piezo buzzer connected through Velleman VMA411 Mos Driving Module (see www.velleman.eu).
- Signal generator, amplifier, and horn loud speaker, connected through Velleman VMA411 Mos Driving Module (see www.velleman.eu) and relay.
- Power supply (12 VDC battery) and voltage stabilizers

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


