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

// ----------
// FUNCTIONS
// ----------

/** Sets the status of a single LED for a matrix sat upright, pins downward. */
void setLedUpright(int row, int col, bool state) {
  lc.setLed(0, col, 7 - row, state);
}

// ------------------------------
// GAME HOLDER CLASSES & OBJECTS
// ------------------------------

class Snake {
  private:
    int dotsPerSec;
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
