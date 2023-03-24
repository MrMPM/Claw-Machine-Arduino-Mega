//////////////////////////////////////////////////////
//      Arcade Game Claw machine - by Marcin Em     //
//**************************************************//
//      ***Version with added MANUAL MODE***        //
//           Uses Nema 17 stepper motors            //
//     (1 for X axis, 2 for Y axis, 1 for Z axis)   //
// and hobby servo for operating the claw mechanism //
// 2 endstop switches for X and Y axes, 1 for Z axis//
// LED lights connected through a relay - to pin 23 //
//     Arcade joystick controls gantry movement     //
//    (not analog - consists of 4 micro switches)   //
// Credit button - it's IR gate, that detects coin, //
//       while it's passing through coin slot       //
//////////////////////////////////////////////////////

#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <elapsedMillis.h>

// Declare pins for stepper motors and endstop switches
const int xStepPin = 2;
const int xDirPin = 3;
const int xEndstop1 = 4;
const int xEndstop2 = 5;
const int yStepPin1 = 6;
const int yDirPin1 = 7;
const int yEndstop1 = 8;
const int yStepPin2 = 9;
const int yDirPin2 = 10;
const int yEndstop2 = 11;
const int zStepPin = 12;
const int zDirPin = 13;
const int zEndstop = 14;

// Declare pins for servo and pushbuttons
const int servoPin = 15;
const int startButton = 16;
const int clawButton = 17;
const int leftButton = 18;
const int rightButton = 19;
const int forwardButton = 20;
const int backwardButton = 21;
const int creditButton = 22;

// Declare pin for alphanumeric display
const int lcdAddress = 0x27;

// Declare variables for LCD
LiquidCrystal_I2C lcd(lcdAddress, 20, 4);

// Declare pin for LED lights
const int ledPin = 23;

// Declare variables for stepper motors
AccelStepper xStepper(AccelStepper::DRIVER, xStepPin, xDirPin);
AccelStepper yStepper1(AccelStepper::DRIVER, yStepPin1, yDirPin1);
AccelStepper yStepper2(AccelStepper::DRIVER, yStepPin2, yDirPin2);
AccelStepper zStepper(AccelStepper::DRIVER, zStepPin, zDirPin);

// Declare variable for servo motor
Servo clawServo;

// Declare variables for pushbutton and limit switch states
int startButtonState = 0;
int clawButtonState = 0;
int creditButtonState = 0;
int leftButtonState = 0;
int rightButtonState = 0;
int forwardButtonState = 0;
int backwardButtonState = 0;
int xEndstop1State = 0;
int xEndstop2State = 0;
int yEndstop1State = 0;
int yEndstop2State = 0;
int zEndstopState = 0;

// Declare variables for game state
int creditCount = 0;
bool gameStarted = false;
bool manualMode = false; // Add a new variable for manual mode

// Declare an additional variable to track the previous time
unsigned long previousMillis = 0;
unsigned long startButtonMillis = 0; // Add a new variable to track the time the start button is held
elapsedMillis gameTimer;

void homeSteppers();
void moveZAxisUp();
void moveZAxisDown();
void moveXAxis();
void moveYAxis();
void openClaw();
void closeClaw();
void endGame();
void manualControls();

/////////////////////  SETUP /////////////////////////

void setup() {
  // Set pin modes for pushbuttons, limit switches, and LED lights
  pinMode(startButton, INPUT_PULLUP);
  pinMode(clawButton, INPUT_PULLUP);
  pinMode(creditButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(rightButton, INPUT_PULLUP);
  pinMode(forwardButton, INPUT_PULLUP);
  pinMode(backwardButton, INPUT_PULLUP);
  pinMode(xEndstop1, INPUT_PULLUP);
  pinMode(xEndstop2, INPUT_PULLUP);
  pinMode(yEndstop1, INPUT_PULLUP);
  pinMode(yEndstop2, INPUT_PULLUP);
  pinMode(zEndstop, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Set up the LCD
  lcd.init();
  lcd.backlight();

  // Set up the stepper motors
  xStepper.setMaxSpeed(400);
  xStepper.setAcceleration(200);
  yStepper1.setMaxSpeed(400);
  yStepper1.setAcceleration(200);
  yStepper2.setMaxSpeed(400);
  yStepper2.setAcceleration(200);
  zStepper.setMaxSpeed(200);
  zStepper.setAcceleration(100);

  // Set up the servo motor
  clawServo.attach(servoPin);

  // Turn on LED lights
  digitalWrite(ledPin, HIGH);

  // Home the stepper motors
  homeSteppers();

  // Set the initial credit count
  creditCount = 0;

  // Print the initial message on the LCD
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("          ");
  lcd.setCursor(3, 1);
  lcd.print("CLAW  MACHINE!");
  lcd.setCursor(4, 2);
  lcd.print("Insert Coin");
  lcd.setCursor(1, 3);
  lcd.print("To Start The Game");
}

/////////////////// HOME ALL AXES ////////////////////

void homeSteppers() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Claw Machine Homing");
  lcd.setCursor(4, 2);
  lcd.print("Please wait");

  // Home the Z axis stepper motor
  zStepper.setSpeed(-200);
  while (digitalRead(zEndstop) == HIGH) {
    zStepper.runSpeed();
  }
  zStepper.setCurrentPosition(0);

  // Home the X axis stepper motor
  xStepper.setSpeed(-400);
  while (digitalRead(xEndstop1) == HIGH) {
    xStepper.runSpeed();
  }
  xStepper.setCurrentPosition(0);

  // Home the Y axis stepper motors
  yStepper1.setSpeed(-400);
  yStepper2.setSpeed(400);
  while (digitalRead(yEndstop1) == HIGH) {
    yStepper1.runSpeed();
    yStepper2.runSpeed();
  }
  yStepper1.setCurrentPosition(0);
  yStepper2.setCurrentPosition(0);

  lcd.clear();
}

/////////////// MOVE Z AXIS (CLAW) UP ////////////////

void moveZAxisUp() {

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("..CLAW GOING UP..");

  zStepper.setSpeed(-200);
  while (digitalRead(zEndstop) == HIGH) {
    zStepper.runSpeed();
  }
  zStepper.setSpeed(0);
  lcd.clear();
}

/////////////// MOVE Z AXIS (CLAW) DOWN //////////////

void moveZAxisDown() {

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("..CLAW GOING DOWN..");

  zStepper.setSpeed(200);
  unsigned long startTime = millis();
  while (millis() - startTime < 4000) {
    zStepper.runSpeed();
  }
  zStepper.setSpeed(0);
  lcd.clear();
}

//////////////////// MOVE X AXIS /////////////////////

void moveXAxis() {

  while (digitalRead(leftButton) == LOW && digitalRead(xEndstop1) == HIGH) {
    xStepper.setSpeed(-400);
    xStepper.runSpeed();
  }
  xStepper.setSpeed(0);

  while (digitalRead(rightButton) == LOW && digitalRead(xEndstop2) == HIGH) {
    xStepper.setSpeed(400);
    xStepper.runSpeed();
  }
  xStepper.setSpeed(0);
}

///////////////////// MOVE Y AXIS ////////////////////

void moveYAxis() {

  while (digitalRead(forwardButton) == LOW && digitalRead(yEndstop1) == HIGH) {
    yStepper1.setSpeed(400);
    yStepper2.setSpeed(-400);
    yStepper1.runSpeed();
    yStepper2.runSpeed();
  }
  yStepper1.setSpeed(0);
  yStepper2.setSpeed(0);

  while (digitalRead(backwardButton) == LOW && digitalRead(yEndstop2) == HIGH) {
    yStepper1.setSpeed(-400);
    yStepper2.setSpeed(400);
    yStepper1.runSpeed();
    yStepper2.runSpeed();
  }
  yStepper1.setSpeed(0);
  yStepper2.setSpeed(0);
}

///////////////////// OPEN CLAW //////////////////////

void openClaw() {

  // Open the claw
  clawServo.write(180);
  delay(1500);
}

///////////////////// CLOSE CLAW /////////////////////

void closeClaw() {

  // Close the claw
  clawServo.write(0);
  delay(1000);
}
/////////////////// MANUAL CONTROLS //////////////////////
//                To enter MANUAL MODE                  //
//      press and hold START button for 3 seconds.      //
//                To exit MANUAL MODE                   //
//      press and hold START button for 2 seconds.      //
// No credits must be available to toggle between modes //
//////////////////////////////////////////////////////////

void manualControls() {

  // When Claw button is kept pressed, 
  // Joystick controls Z axis and CLAW servo
  
  if (digitalRead(clawButton) == LOW) {

    // Control the claw open and close
    if (digitalRead(leftButton) == LOW) {
      clawServo.write(180); // Open the claw
      delay(1500);
    }

    if (digitalRead(rightButton) == LOW) {
      clawServo.write(0); // Close the claw
      delay(1000);
    }

    // Control the claw up and down
    if (digitalRead(forwardButton) == LOW) {
      lcd.clear();
      lcd.setCursor(1, 1);
      delay(100);
      lcd.print("..CLAW GOING UP..");

      zStepper.setSpeed(-200);

      while (digitalRead(zEndstop) == HIGH) {
        zStepper.runSpeed();
      }
      zStepper.setSpeed(0);
    }

    if (digitalRead(backwardButton) == LOW) {
      lcd.clear();
      lcd.setCursor(1, 1);
      delay(100);
      lcd.print("..CLAW GOING DOWN..");

      zStepper.setSpeed(200);
      unsigned long startTime = millis();
      while (millis() - startTime < 4000) {
        zStepper.runSpeed();
      }
      zStepper.setSpeed(0);
    }

  } else {

    // When CLAW button isn't pressed,
    // Joystick controls gantry movements on X and Y axes

    while (digitalRead(leftButton) == LOW && digitalRead(xEndstop1) == HIGH) {
      xStepper.setSpeed(-400);
      xStepper.runSpeed();
    }
    xStepper.setSpeed(0);

    while (digitalRead(rightButton) == LOW && digitalRead(xEndstop2) == HIGH) {
      xStepper.setSpeed(400);
      xStepper.runSpeed();
    }
    xStepper.setSpeed(0);

    while (digitalRead(forwardButton) == LOW && digitalRead(yEndstop1) == HIGH) {
      yStepper1.setSpeed(400);
      yStepper2.setSpeed(-400);
      yStepper1.runSpeed();
      yStepper2.runSpeed();
    }
    yStepper1.setSpeed(0);
    yStepper2.setSpeed(0);

    while (digitalRead(backwardButton) == LOW && digitalRead(yEndstop2) == HIGH) {
      yStepper1.setSpeed(-400);
      yStepper2.setSpeed(400);
      yStepper1.runSpeed();
      yStepper2.runSpeed();
    }
    yStepper1.setSpeed(0);
    yStepper2.setSpeed(0);
  }
}

//////////////////// END THE GAME ////////////////////

void endGame() {

  lcd.clear();

  // Open the claw
  openClaw();

  // Lower the claw
  moveZAxisDown();

  delay(1000);

  // Close the claw
  closeClaw();

  // Raise the claw
  moveZAxisUp();

  // Home the stepper motors
  homeSteppers();

  delay(1000);

  lcd.setCursor(1, 1);
  lcd.print("Releasing prize...");

  // Open the claw to release the prize
  openClaw();

  delay(1500);

  // Close the claw
  closeClaw();

  lcd.clear();
}

/////////////////// PLAY THE GAME ////////////////////

void loop() {

  // Check if the start button is held for 3 seconds to enter MANUAL MODE
  if (creditCount == 0 && digitalRead(startButton) == LOW) {
    if (startButtonMillis == 0) {
      startButtonMillis = millis();
    } else if (millis() - startButtonMillis >= 3000) {
      manualMode = !manualMode; // Toggle manual mode
      startButtonMillis = 0;
      delay(500); // debounce the button
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MANUAL MODE");
    }
  } else {
    startButtonMillis = 0;
  }

  if (manualMode) {

    // Check if the start button is held for 2 seconds to exit MANUAL MODE
    if (creditCount == 0 && digitalRead(startButton) == LOW) {
      if (startButtonMillis == 0) {
        startButtonMillis = millis();
      } else if (millis() - startButtonMillis >= 2000) {
        manualMode = !manualMode; // Toggle auto mode
        startButtonMillis = 0;
        delay(500); // debounce the button
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("AUTO MODE");
      }
    } else {
      manualControls(); // Call manualControls() function in manual mode
    }
  } else {

    // Check for credits being inserted
    if (digitalRead(creditButton) == LOW) {
      creditCount++;
      delay(100); // debounce the button
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Coin inserted");
      lcd.setCursor(5, 1);
      lcd.print("Credits: ");
      lcd.print(creditCount);
      lcd.setCursor(0, 3);
      lcd.print("Press START to begin");
      delay(1000);
    }

    // Check for game starting
    if (!gameStarted && creditCount > 0 && digitalRead(startButton) == LOW) {

      // Display a message on the LCD when the game starts
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Game starting...");
      lcd.setCursor(4, 2);
      lcd.print("GET READY!");
      delay(1000);
      lcd.clear();

      gameStarted = true;
      gameTimer = 0;
      creditCount--;
    }

    // Control the stepper motor if the game is started
    if (gameStarted) {

      moveXAxis();
      moveYAxis();

      // Count down timer
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;

        // Calculate and display the remaining time on the LCD
        int remainingTime = 30 - (gameTimer / 1000);
        lcd.setCursor(0, 0);
        lcd.print("Use JOYSTICK to MOVE");
        lcd.setCursor(1, 1);
        lcd.print("Press CLAW to GRAB");
        lcd.setCursor(5, 2);
        lcd.print("Good Luck!");
        lcd.setCursor(3, 3);
        lcd.print("Time left: ");
        lcd.print(remainingTime);
        lcd.print("s ");
      }
    }

    // Check for game ending
    if (gameStarted && (gameTimer >= 30000 || digitalRead(clawButton) == LOW)) {
      gameStarted = false;

      // Reset the game timer and previousMillis
      gameTimer = 0;
      previousMillis = 0;

      // End the game
      endGame();

      // Wait for player to insert coin or press start button
      bool waitingForInput = true; // add flag variable
      while (waitingForInput) { // loop until flag variable is false
        lcd.setCursor(5, 0);
        lcd.print("       ");
        lcd.setCursor(3, 1);
        lcd.print("CLAW  MACHINE!");
        lcd.setCursor(5, 2);
        lcd.print("Credits: ");
        lcd.print(creditCount);
        lcd.setCursor(0, 3);
        lcd.print("Press START to begin");

        if (digitalRead(creditButton) == LOW || digitalRead(startButton) == LOW) {
          waitingForInput = false; // set flag variable to exit loop
        }
      }
    }
  }
}