# Timer for Canoe Freestyle 
Timer for Canoe Freestyle competitions which complies with ICF Canoe Freestyle Competition Rules 2019.

Operation is very simple:
- Press a button to start the timer which is running from 45 seconds down to 0. Time is shown on LED dislay.
- One tone is played 10 seconds before end. 
- Two tones are played in the end.
- Blinking "X" is played when the time has finished.
- The timer is ready for the new run immediately after finishing the time.

See the video on YouTube https://youtu.be/Ev1N3gt9AcE

Software is running on Arduino Uno, which is connected to LED display, loud speaker and switch. The switch is used to start the timer.  

Time is outputted to serial port of the Arduino Uno, which enables addtional functionality outside of the Timer. 
