// Brandon Namgoong
// TEJ4M1
// FPT
// 2026-06-##

#include <LedControl.h>

// ----------------
// PIN DEFINITIONS
// ----------------

#define PIN_BUT_L_SW 3
#define PIN_BUT_L_SE 4
#define PIN_BUT_L_NE 5

#define PIN_BUT_R_DN 11
#define PIN_BUT_R_UP 12

#define PIN_LED 2
#define PIN_PZO 6

#define PIN_MAT_DIN 8
#define PIN_MAT_CLK 10
#define PIN_MAT_CS 9

// ------------------
// GLOBALS AND STUFF
// ------------------

LedControl lc = LedControl(PIN_MAT_DIN, PIN_MAT_CLK, PIN_MAT_CS); // LED matrix object

bool ledStatus[8][8] = {0}; // matrix tracker
bool buttonStatus[5] = {0}; // button state tracker, in order of L_SW, L_SE, L_NE, R_DN, R_UP
bool buttonHoldStatus[5] = {0}; // button hold tracker

// ----------
// FUNCTIONS
// ----------

/**
* Sets the status of a single PIN_LED for a matrix sat upright, pins downward. Updates the global status tracking array.
* Must use everywhere in this project.
*/
void setLedUpright(uint8_t row, uint8_t col, bool state) {
  lc.setLed(0, col, 7 - row, state); // led
  ledStatus[row][col] = state; // tracker array
}

// ------------------------------
// GAME HOLDER CLASSES & OBJECTS
// ------------------------------

/** Utility enum for direction specificaiton. */
enum Direction : uint8_t {
  UP, DOWN, LEFT, RIGHT
};

/** Everything needed to run Pong. */
class Pong {
  private:
    uint8_t dotsPerSec; // speed of ball
    uint8_t platLength; // platform length

    uint8_t locPlatL[2]; // left platform location, topmost dot
    uint8_t locPlatR[2]; // right platform location
  
    uint8_t locBall[2]; // ball location
  
  public:
    Pong(uint8_t speed, uint8_t platLength) {
      dotsPerSec = speed;
      this->platLength = platLength;
    }

    /** Resets the game to start state. */
    void reset() {
      locPlatL[0] = locPlatL[1] = 0; // top left
      locPlatR[0] = 0; // top right
      locPlatR[1] = 7;

      locBall[0] = locBall[1] = 5; // first ever location (5, 5)
    }

    /** Game loop. Call every iteration. Takes in last time delta in ms. */
    void loop(unsigned int delta) {
      
    }
};

/** Everything needed to run Snake. */
class Snake {
  private:
    uint8_t dotsPerSec; // speed of snake

    uint8_t locSnake[2]; // snake head location
    uint8_t length; // snake length, max 8 x 8 = 64
    Direction dir; // snake heading

    uint8_t locApple[2]; // apple location
  
  public:
    Snake(uint8_t speed) {
      dotsPerSec = speed;
      reset();
    }

    /** Resets the game to start state. */
    void reset() {
      locSnake[0] = 1; // spawnpoint (1, 3)
      locSnake[1] = 3;
      length = 3;
      dir = RIGHT;

      locApple[0] = locApple[1] = 5; // first ever location (5, 5)
    }

    /** Game loop. Call every iteration. Takes in last time delta in ms. */
    void loop(unsigned int delta) {
      
    }
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // button input stack

  // L_SW
  if (buttonStatus[0] == 1) buttonStatus[0] = 0; // always collapse immediately
  if (digitalRead(PIN_BUT_L_SW) == HIGH) { // if button press
    if (buttonHoldStatus[0] == 0) { // check if new press
      buttonStatus[0] = 1;
      buttonHoldStatus[0] = 1;
    }
  } else {
    buttonHoldStatus[0] = 0; // signal release
  }
  // L_SE
  if (buttonStatus[1] == 1) buttonStatus[1] = 0; // always collapse immediately
  if (digitalRead(PIN_BUT_L_SE) == HIGH) { // if button press
    if (buttonHoldStatus[1] == 0) { // check if new press
      buttonStatus[1] = 1;
      buttonHoldStatus[1] = 1;
    }
  } else {
    buttonHoldStatus[1] = 0; // signal release
  }
  // L_NE
  if (buttonStatus[2] == 1) buttonStatus[2] = 0; // always collapse immediately
  if (digitalRead(PIN_BUT_L_NE) == HIGH) { // if button press
    if (buttonHoldStatus[2] == 0) { // check if new press
      buttonStatus[2] = 1;
      buttonHoldStatus[2] = 1;
    }
  } else {
    buttonHoldStatus[2] = 0; // signal release
  }
  // R_DN
  if (buttonStatus[3] == 1) buttonStatus[3] = 0; // always collapse immediately
  if (digitalRead(PIN_BUT_R_DN) == HIGH) { // if button press
    if (buttonHoldStatus[3] == 0) { // check if new press
      buttonStatus[3] = 1;
      buttonHoldStatus[3] = 1;
    }
  } else {
    buttonHoldStatus[3] = 0; // signal release
  }
  // R_UP
  if (buttonStatus[4] == 1) buttonStatus[4] = 0; // always collapse immediately
  if (digitalRead(PIN_BUT_R_UP) == HIGH) { // if button press
    if (buttonHoldStatus[4] == 0) { // check if new press
      buttonStatus[4] = 1;
      buttonHoldStatus[4] = 1;
    }
  } else {
    buttonHoldStatus[4] = 0; // signal release
  }
}
