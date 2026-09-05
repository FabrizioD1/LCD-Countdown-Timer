#include <LiquidCrystal.h>

// --- INITIALIZATION ---

// LCD pin initialization
int rs = 7;
int en = 8;
int d4 = 9;
int d5 = 10;
int d6 = 11;
int d7 = 12;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

// component pin assignment
int decButton = 2;
int incButton = 3;
int confirmButton = 4;
int buzzPin = 5;

// beeping configuration
int separation = 125; // time betweeen iterations in miliseconds

// DO NOT CHANGE!!! These values are updated during execution
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

int decPrev = 0;
int incPrev = 0;
int confirmPrev = 0;

// --- SETUP ---

void setup() {
  // for all the buttons, we set them up as pullup to give them a
  // pullup resistor
  pinMode(decButton, INPUT_PULLUP);
  pinMode(incButton, INPUT_PULLUP);
  pinMode(confirmButton, INPUT_PULLUP);

  // set up the buzzer
  pinMode(buzzPin, OUTPUT);

  // final setup
  lcd.begin(16,2); // 16 columns, 2 rows
  Serial.begin(9600);
}

// --- WELCOME SCREEN ---

void startScreen() {
  // shows a welcome screen to the user.
  // the welcome screen prompts the user
  // to press any button to begin.

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome to Timer!");
  lcd.setCursor(0,1);
  lcd.print("Push any button!");
}

// --- TIMER & BEEPING ---

void runTimer() {
  // updates the LCD display to decrement the timer.

  // LCD setup
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time left:");
  lcd.setCursor(0,1);
  lcd.print(getTimeString());

  // determine how many total seconds to iterate over
  while (days > 0 || hours > 0 || minutes > 0 || seconds > 0) {
    delay(1000);
    seconds--;

    // if seconds try going negative, we remove 1 minute since a minute has passed
    if (seconds < 0) {
      seconds = 0;
      minutes--;

      // if minutes try going negative, an hour has completed so we remove an hour
      if (minutes < 0) {
        minutes = 0;
        hours--;

        // if hours try going negative, the current day has completed to we remove a day
        if (hours < 0) {
          hours = 0;
          days--;

          if (days < 0) {
            days = 0;
          }

          // the following 3 else statements account for setting the lower time quantities
          // to their max when the current highest unit of time decreases
          else {
            hours = 23;
            minutes = 59;
            seconds = 59;
          }
        }
        else {
          minutes = 59;
          seconds = 59;
        }
      }
      else {
        seconds = 59;
      }
    }

    // update LCD screen with new time
    lcd.setCursor(0,1);
    lcd.print(getTimeString());
  }
}

void beepUntilStop() {
  // runs when the timer reaches 00:00:00:00.
  // uses configuration to beep at regular intervals
  // and has a pause after a set amount of intervals

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press any button");
  lcd.setCursor(0,1);
  lcd.print("to stop!");

  // these variables act as a rising edge checker for 
  // all buttons combined
  bool prevState = anyButtonPressed();
  bool currentState = false; 

  // execute while none of the buttons are on rising edge.
  // we loop like this since pressing any button is meant to
  // stop the beeping
  while (!(prevState == false && currentState == true)) {
    
    // updates current state of all buttons
    currentState = anyButtonPressed();
    if (prevState == false && currentState == true) 
      return; // ends the function if any button goes rising edge (stops beeping)
    
    // update previous value since the rising edge check has already been done
    prevState = currentState; 
    digitalWrite(buzzPin, HIGH); // turn on buzzer

    delay(separation); // delay between beeps

    // update current state of all buttons again and check rising edge again
    currentState = anyButtonPressed();

    // this time, since the buzzer is ON, we have to ensure it is shut off before ending the function
    if (prevState == false && currentState == true) {
      digitalWrite(buzzPin, LOW); 
      return;
    }

    // update previous value again since the rising edge check has passed
    prevState = currentState; 
    digitalWrite(buzzPin, LOW); // turn off buzzer

    delay(separation); // delay between beeps
  }
  
}

// --- BUTTON USER INPUT ---

void buttonAdjustableIntInput(int* valToUpdate, String label, int min, int max) {
  // allows the user to use the increment and decrement buttons to
  // input a value in the range min <= x <= max.
  // the label is used to indicate if the user is inputting days, hours,
  // minutes, or seconds

  // update the display on the LCD to reflect that user input is being
  // collected
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter "+label+":");
  lcd.setCursor(0,1);
  lcd.print(getTimeString()); // shows currently configured time

  // these variables are used to determine when each configuration button
  // is in the rising edge position
  int decPrev = getButtonReading(decButton);
  int incPrev = getButtonReading(incButton);
  int conPrev = getButtonReading(confirmButton);

  int decNow = 0;
  int incNow = 0;
  int conNow = 0;

  while (!(conPrev == 0 && conNow == 1)) { // run while confirm button is NOT rising edge
    
    // update the current state of each button
    decNow = getButtonReading(decButton);
    incNow = getButtonReading(incButton);
    conNow = getButtonReading(confirmButton);

    if (conPrev == 0 && conNow == 1) {return;} // end the function if confirm is on rising edge

    // decrement
    if (decPrev == 0 && decNow == 1) { // run when decrement button is rising edge
      *valToUpdate = *valToUpdate > min ? *valToUpdate - 1 : max; // wraps around if overflow
      lcd.setCursor(0,1);
      lcd.print(getTimeString());
    }

    // increment
    if (incPrev == 0 && incNow == 1) { // run when decrement button is rising edge
      *valToUpdate = *valToUpdate < max ? *valToUpdate + 1 : min; // wraps around if underflow
      lcd.setCursor(0,1);
      lcd.print(getTimeString());
    }

    // update old values for the buttons
    decPrev = decNow;
    incPrev = incNow;
    conPrev = conNow;

  }
}

// --- TIME FORMATTING ---

String getTimeString() {
  // uses the current values of days, hours, minutes, and seconds,
  // and returns a string in the format dd:hh:mm:ss

  String num_days = formatTimePart(days);
  String hrs = formatTimePart(hours);
  String mins = formatTimePart(minutes);
  String secs = formatTimePart(seconds);

  return num_days + ":" + hrs + ":" + mins + ":" + secs;
}

String formatTimePart(int value) {
  // takes an integer for an amount of days/hours/minutes/seconds
  // and returns it as a string. if the value is a single digit,
  // adds a padding 0
  String result = value >= 10 ? String(value) : "0" + String(value);
  return result;
}

// --- BUTTON HANDLING FUNCTIONS ---

bool anyButtonPressed() {
  // returns True if any button is pressed
  return getButtonReading(incButton) || getButtonReading(decButton) || getButtonReading(confirmButton);
}

void waitForAnyPress() {
  // waits until any button is pressed, then waits until all buttons are unpressed
  while (!getButtonReading(incButton) && !getButtonReading(decButton) && !getButtonReading(confirmButton)) {} // goes until a button is pressed
  while (getButtonReading(incButton) || getButtonReading(decButton) || getButtonReading(confirmButton)) {} // goes until ALL buttons are off
  return;
}

void waitForDepress() {
  // holds the program until all buttons are unpressed
  while (getButtonReading(incButton) || getButtonReading(decButton) || getButtonReading(confirmButton)) {}// goes until ALL buttons are off
  return;
}

int getButtonReading(int buttonPin) {
  // digitalRead() gives 0 for HIGH and 1 for LOW with the current
  // circuit configuration. this function acts as a better input
  // function which instead returns 1 when HIGH, and 0 when LOW

  return abs(digitalRead(buttonPin)-1);
}

// --- EXECUTION LOOP ---

void loop() {
  // display the welcome screen
  startScreen();
  waitForAnyPress(); // pressing any button moves user past welcome screen

  // use buttons to take user input
  buttonAdjustableIntInput(&days, "days", 0, 99);
  waitForDepress(); // to avoid any issues wait for user to stop pressing buttons
                    // before continuing

  buttonAdjustableIntInput(&hours, "hours", 0, 23);
  waitForDepress();

  buttonAdjustableIntInput(&minutes, "minutes", 0, 59);
  waitForDepress();

  buttonAdjustableIntInput(&seconds, "seconds", 0, 59);
  waitForDepress();

  // run the timer and beep when when it runs out
  runTimer();

  beepUntilStop();
  waitForDepress();
}
