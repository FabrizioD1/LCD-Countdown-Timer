# LCD Countdown Timer
An Arduino-based embedded countdown timer with a Liquid Crystal Display (LCD) user interface.

# Overview
This project allows the user to configure a countdown timer in the format ```DD:HH:MM:SS``` through push buttons which are connected to an Arduino UNO R3 Microcontroller. An LCD screen is used as the user interface to give feedback to the user while they input the desired time, and to denote how much time remains on the timer while it counts down. When the time runs out, a buzzer beeps at a regular interval, and pressing any button will disable the buzzer, and restart execution of the program. 

# Features
- Adjustable display contrast using potentiometer
- Simple interface with few inputs
- Supports overflow and underflow when inputting time (for example, decrementing 1 second while the seconds are currently set to 0 seconds will underflow around to 59 seconds)
- State machine logic to facilitate moving between menus

# Hardware
- Arduino UNO R3 Microcontroller with USB Cable
- LCD1602 Module
- 3 Push Buttons
- Active Buzzer
- 10k Ohm Potentiometer
- Breadboard and Jumper Wires

# Software
- Arduino IDE
- C++
- LiquidCrystal library

# How It Works
### 1. Startup
When first running the Arduino code, the LCD screen will display a welcome message to the user. Pressing any of the three push buttons will advance the user to the next state of the project.

### 2. Time Input
The time input is split into 4 different screens. One for days, one for hours, and so on. At this stage, the increment and decrement buttons are used to adjust the respective unit of time. The increment button, when pressed, increases the currently selected time unit by 1 unit, while the decrement button decreases it by 1 unit. The LCD screen will update to reflect any changes to the current time unit.

Each time unit supports a specific range of values, as follows:

```
Days: 0-99
Hours: 0-23
Minutes: 0-59
Seconds: 0-59
```

Attempting to decrement past the lower limit (which is 0 for all the units) will underflow the current value to the maximum value allowed for that unit. For example, if we are currently inputting hours, and we attempt to decrement by 1 when the hours are currently set to 0 hours, the value will underflow to 23 hours. Similarly, attempting to go above the upper limit will result in overflow back to the lower limit. For example, trying to add 1 second to 59 seconds will overflow to 0 seconds. Overflow and underflow will not modify any adjacent time units to prevent unintentional modification of those units.

Once the user is satisfied with their selection for the current time unit, they can press the confirm button to move on to the next unit. Upon using the confirm button at the seconds stage, the user advances to the next stage.

### 3. Countdown
Here is where the timer actually counts down. The LCD screen will show the remaining time as the Arduino handles counting down. Once the timer expires, the buzzer will go off at a regular interval, and the LCD screen will ask the user to press any button to shut off the timer. Shutting off the timer then takes the user back to the welcome screen, where they can then begin to set another timer.

# Schematic
The schematic for the project was created using KiCad. A KiCad schematic file is included in the ```schematic/``` directory, as well as a PDF version which may be easier to access. A preview of the schematic is available here:

<img width="946" height="617" alt="image" src="https://github.com/user-attachments/assets/bc292d47-872e-4091-86f6-f888943b0c6b" />

# Prototype
A picture of the final build, along with a short demonstration video are available in the ```visuals/``` directory.

# What I Learned From This Project
- Functions are a great way to handle state machine logic
- LCD writing often requires clearing beforehand to prevent stray characters from remaining on screen
- To prevent holding down a button being registered as many button presses, one can check for when the button is on the rising edge of its output signal so that only the pressing of the button counts as a press
- This was my first time using KiCad to make a schematic, so I watched some tutorials in preparation for this

# Potential Future Improvements
- Implement button debouncing so that button presses don't sometimes register as multiple presses
- Adding the ability to move backwards through the menus
- Pressing a certain button (or any button) during the timer countdown either pauses or ends the timer
- Adding additional related functionalities, such as a stopwatch
