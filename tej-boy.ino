// Brandon Namgoong
// TEJ4M1
// FPT
// 2026-06-##

#include <LedControl.h>

// ----------------
// PIN DEFINITIONS
// ----------------

#define BUT_L_SW 3
#define BUT_L_SE 4
#define BUT_L_NE 5

#define BUT_R_DN 11
#define BUT_R_UP 12

#define LED 2
#define PZO 6

#define MAT_DIN 8
#define MAT_CLK 10
#define MAT_CS 9

// ------------------
// GLOBALS AND STUFF
// ------------------

LedControl lc = LedControl(MAT_DIN, MAT_CLK, MAT_CS);
bool ledStatus[8][8] = {0};

// ----------
// FUNCTIONS
// ----------

/**
* Sets the status of a single LED for a matrix sat upright, pins downward. Updates the global status tracking array.
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

/** Utility enum defining all 5 buttons. */
enum Button : uint8_t {
  L_SW, L_SE, L_NE, R_DN, R_UP
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
    Snake(uint8_t speed) { // specifiable snake speed
      dotsPerSec = speed;
      reset();
    }

    /** Resets the game to start state. */
    void reset() {
      locSnake[0] = 1;
      locSnake[1] = 3;
      length = 3;
      dir = RIGHT;

      locApple[0] = locApple[1] = 0;
    }

    /** Game loop. Takes in last time delta in ms. */
    void loop(int delta) {
      
    }
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
