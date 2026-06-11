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

// ----------
// CONSTANTS
// ----------

const uint8_t SIG_STOP_GAME = 1;
const uint8_t SIG_GAMEID_1 = 2;
const uint8_t SIG_GAMEID_2 = 3;

// ------------------
// GLOBALS AND STUFF
// ------------------

LedControl lc = LedControl(PIN_MAT_DIN, PIN_MAT_CLK, PIN_MAT_CS); // LED matrix object

bool ledStatus[8][8] = {0}; // matrix tracker
bool buttonStatus[5] = {0}; // button state tracker, in order of L_SW, L_SE, L_NE, R_DN, R_UP
bool buttonHoldStatus[5] = {0}; // button hold tracker

uint8_t nowGameID = 0; // what game ID is running right now, 0 is no game running

unsigned long lastMillis = 0; // previous loop timestamp

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

/** Clears state of device and unloads any running game. */
void clear() {
  nowGameID = 0; // unload game

  lc.clearDisplay(0);
  for (uint8_t i = 0; i < 8; i++) { // reset tracker array
    for (uint8_t j = 0; j < 8; j++) {
      ledStatus[i][j] = 0;
    }
  }

  digitalWrite(PIN_LED, HIGH);
}

// ------------------------------
// GAME HOLDER CLASSES & OBJECTS
// ------------------------------

/** Utility enum for direction specificaiton. */
enum Direction : uint8_t {
  UP, DOWN, LEFT, RIGHT
};

/** Everything needed to run Pong, game ID 1. */
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

/** Everything needed to run Snake, game ID 2. */
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

// -------------
// GAME OBJECTS
// -------------

Pong gamePong = Pong(3, 3);
Snake gameSnake = Snake(3);

// --------
// ARDUINO
// --------

void setup() {
  // pins
  pinMode(PIN_BUT_L_SW, INPUT);
  pinMode(PIN_BUT_L_SE, INPUT);
  pinMode(PIN_BUT_L_NE, INPUT);

  pinMode(PIN_BUT_R_DN, INPUT);
  pinMode(PIN_BUT_R_UP, INPUT);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_PZO, OUTPUT);

  // serial
  Serial.begin(9600);

  // init
  clear();
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

  // game load, unload, and reset stack

  if (Serial.available()) {
    uint8_t nextSig = Serial.parseInt();

    switch (nextSig) { // condition thru all possible signals
      case SIG_STOP_GAME:
        clear();
        break;
      case SIG_GAMEID_1: // pong
        nowGameID = 1;
        gamePong.reset();
        break;
      case SIG_GAMEID_2: // snake
        nowGameID = 2;
        gameSnake.reset();
        break;
    }
  }

  // game loop stack

  unsigned long now = millis();
  unsigned int delta = now - lastMillis; // find one consistent time delta

  switch (nowGameID) { // condition thru all games
    case 1:
      gamePong.loop(delta);
      break;
    case 2:
      gameSnake.loop(delta);
      break;
  }

  lastMillis = now; // track this timestamp
}
