// Brandon Namgoong
// TEJ4M1
// FPT
// 2026-06-##

// This sketch requires the LedControl library to control the MAX7219 chip.
// More info here: https://docs.arduino.cc/libraries/ledcontrol/.
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

bool ledStatus[8][8] = {false}; // matrix tracker
bool buttonStatus[5] = {false}; // button state tracker, in order of L_SW, L_SE, L_NE, R_DN, R_UP
bool buttonHoldStatus[5] = {false}; // button hold tracker

uint8_t nowGameID = 0; // what game ID is running right now, 0 is no game running

unsigned long lastMillis = 0; // previous loop timestamp

// ------
// ENUMS
// ------

/** Direction specification. */
enum Direction : uint8_t {
  UP, DOWN, LEFT, RIGHT,
  NOEA, NOWE, SOWE, SOEA // cause apparently "SE" is a built-in macro WHY
};

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

/** Turns off all LEDs on the matrix and wipes the tracker accordingly. */
void clearMatrix() {
  lc.clearDisplay(0);
  for (uint8_t i = 0; i < 8; i++) { // reset tracker array
    for (uint8_t j = 0; j < 8; j++) {
      ledStatus[i][j] = false;
    }
  }
}

/** Plays a universal game start sound. THIS IS A BLOCKING FUNCTION. */
void playStartSound() {
  for (uint8_t i = 0; i < 3; i++) { // blocking
    tone(PIN_PZO, 880); // A5
    delay(500);
    noTone(PIN_PZO);
    delay(500);
  }
  tone(PIN_PZO, 1760, 500); // A6; non-blocking, tone will be heard 500 ms into gameplay
}

/** Play a universal game fail sound. THIS IS A BLOCKING FUNCTION. */
void playFailSound() {
  tone(PIN_PZO, 165); // E3
  delay(1000);
  tone(PIN_PZO, 117); // A2
  delay(2000);
  noTone(PIN_PZO);
}

/** Returns an inverted direction of the specified direction based on given mode. FALSE inverts in vertical, TRUE inverts in horizontal. */
Direction invertDirection(Direction dir, bool mode) {
  if (mode) {
    if (dir == UP) return UP;
    if (dir == DOWN) return DOWN;
    if (dir == LEFT) return RIGHT;
    if (dir == RIGHT) return LEFT;

    if (dir == NOEA) return NOWE;
    if (dir == NOWE) return NOEA;
    if (dir == SOWE) return SOEA;
    if (dir == SOEA) return SOWE;
  }
  else {
    if (dir == UP) return DOWN;
    if (dir == DOWN) return UP;
    if (dir == LEFT) return LEFT;
    if (dir == RIGHT) return RIGHT;

    if (dir == NOEA) return SOEA;
    if (dir == NOWE) return SOWE;
    if (dir == SOWE) return NOWE;
    if (dir == SOEA) return NOEA;
  }
}

// ------------------------------
// GAME HOLDER CLASSES & OBJECTS
// ------------------------------

/** Everything needed to run Pong, game ID 1. */
class Pong {
  private:
    // fields

    unsigned int msPerDot; // speed of ball as an inverse function
    uint8_t platLength; // platform length

    int8_t locPlatL; // left platform row, topmost dot
    int8_t locPlatR; // right platform row
  
    uint8_t locBall[2]; // ball location on matrix
    Direction dir; // ball direction

    unsigned int ballMoveTimer; // fills up each delta until reaches threshold to move ball
    bool wasLastDirVert;
    bool shouldBallMove;

    // functions

    /** Render both platforms based on current specified location. */
    void renderPlats() {
      for (uint8_t i = 0; i < platLength; i++) { // render left plat
        setLedUpright(locPlatL + i, 0, true);
      }
      for (uint8_t i = 0; i < platLength; i++) { // render right plat
        setLedUpright(locPlatR + i, 7, true);
      }
    }
  
  public:
    bool isGameOver;

    /** Pong constructor. Specify speed in dots per second. */
    Pong(uint8_t speed, uint8_t platLength) {
      msPerDot = (unsigned int) round(1000.0 / speed); // round off decimals it's probably close enough
      this->platLength = platLength;
    }

    /** Resets the game to start state. */
    void reset() {
      locPlatL = 0; // top
      locPlatR = 0; // top

      locBall[0] = 4; // first ever location (4, 2)
      locBall[1] = 2;
      dir = SOEA;

      ballMoveTimer = 0;

      isGameOver = false;
    }

    /** Intro sequence post-load. THIS IS A BLOCKING FUNCTION. */
    void onIntro() {
      // show stuff
      renderPlats();
      setLedUpright(locBall[0], locBall[1], true);

      playStartSound();
    }

    /** Game over sequence. THIS IS A BLOCKING FUNCTION. */
    void onGameOver() {
      playFailSound();
      clearMatrix(); // clear screen
      delay(500);
    }

    /** Game loop. Call every iteration. Takes in last time delta in ms. */
    void loop(unsigned int delta) {
      // game over stack

      if (locBall[1] == 0 || locBall[1] == 7) { // the ball managing to reach either column 0 or 7 means player did not successfully hit with platform
        isGameOver = true;
        return;
      }

      // ball move-decision and bounce stack

      shouldBallMove = false;

      ballMoveTimer += delta;
      if (ballMoveTimer >= msPerDot) { // it's time
        shouldBallMove = true;
        ballMoveTimer -= msPerDot; // reset and retain any leftover
      }

      if (shouldBallMove) { // collision detection
        // find status
        bool isUp = locBall[0] == 0;
        bool isDown = locBall[0] == 7;
        bool isLeft = ledStatus[locBall[0]][locBall[1] - 1];
        bool isRight = ledStatus[locBall[0]][locBall[1] + 1];

        // short sound on plat hit
        if ((isLeft || isRight) && !wasLastDirVert) tone(PIN_PZO, 1319, 50);

        // edge case: ensure vertical bounce only happens once
        if (wasLastDirVert) {
            if (locBall[1] == 1) dir = (dir == UP) ? NOEA : SOEA;
            if (locBall[1] == 6) dir = (dir == UP) ? NOWE : SOWE;
            wasLastDirVert = false;
          }
        // floor/ceil AND plat edge cases
        else if (isUp && isLeft) { // ceil AND left plat
          dir = SOEA;
        }
        else if (isUp && isRight) { // ceil AND right plat
          dir = SOWE;
        }
        else if (isDown && isLeft) { // floor AND left plat
          dir = NOEA;
        }
        else if (isDown && isRight) { // floor AND right plat
          dir = NOWE;
        }
        // floor/ceil cases
        else if (isUp || isDown) {
          dir = invertDirection(dir, false);
        }
        // plat cases
        else if (isLeft || isRight) {
          // 50-50 randomizer between diag and 1 move vertical
          uint8_t rand = (uint8_t) random(4);

          if (rand < 2) {
            dir = invertDirection(dir, true); // normal diag
            wasLastDirVert = false;
          }
          else {
            dir = (dir == NOEA || dir == NOWE) ? UP : DOWN; // special 1 vertical
            wasLastDirVert = true;
          }
        }
      }

      // ball move-action and render stack

      if (shouldBallMove) {
        setLedUpright(locBall[0], locBall[1], false); // turn off at current location, which will be past location

        switch (dir) { // move ball
          case NOEA:
            locBall[0] -= 1;
            locBall[1] += 1;
            break;
          case NOWE:
            locBall[0] -= 1;
            locBall[1] -= 1;
            break;
          case SOWE:
            locBall[0] += 1;
            locBall[1] -= 1;
            break;
          case SOEA:
            locBall[0] += 1;
            locBall[1] += 1;
            break;
          case UP:
            locBall[0] -= 1;
            break;
          case DOWN:
            locBall[0] += 1;
        }
      }

      setLedUpright(locBall[0], locBall[1], true); // turn on at current location, potentially now new location

      // platform move and render stack

      if (buttonStatus[1] || buttonStatus[2]) { // if change in left plat
        for (uint8_t i = 0; i < 8; i++) {
          setLedUpright(i, 0, false); // clear left column
        }

        if (buttonStatus[1]) locPlatL = min(locPlatL + 1, 8 - platLength); // move down, clip to bottom
        if (buttonStatus[2]) locPlatL = max(locPlatL - 1, 0); // move up, clip to top
      }
      if (buttonStatus[3] || buttonStatus[4]) { // if change in right plat
        for (uint8_t i = 0; i < 8; i++) {
          setLedUpright(i, 7, false); // clear right column
        }

        if (buttonStatus[3]) locPlatR = min(locPlatR + 1, 8 - platLength); // move down, clip to bottom
        if (buttonStatus[4]) locPlatR = max(locPlatR - 1, 0); // move up, clip to top
      }

      renderPlats();
    }
};

/** Everything needed to run Snake, game ID 2. */
class Snake {
  private:
    // fields

    unsigned int msPerDot; // snake as an inverse function

    int8_t snakeHead[2]; // location of snake head, kinda like a virtual tracker
    int8_t snakeBody[64][2]; // every single possible coordinate of every single snake body cell
    uint8_t length; // snake length, max 8 x 8 = 64
    Direction dir; // snake heading

    uint8_t locApple[2]; // apple location
    bool shouldAppleMove;

    unsigned int snakeMoveTimer;
    bool shouldSnakeMove;

    // functions

    /** Propagate snake body from head position based on length. */
    void propagate() {
      for (int8_t i = length - 2; i >= 0; i--) {
        snakeBody[i + 1][0] = snakeBody[i][0];
        snakeBody[i + 1][1] = snakeBody[i][1];
      }

      snakeBody[0][0] = snakeHead[0];
      snakeBody[0][1] = snakeHead[1];
    }

    /** Render snake based on snake body. Set erase to TRUE to act as an eraser instead. */
    void render(bool erase = false) {
      for (uint8_t i = 0; i < length; i++) {
        setLedUpright(snakeBody[i][0], snakeBody[i][1], !erase);
      }
    }

    /** Set a new random location for the apple. */
    void relocateRandom() {
      uint8_t emptyCells[64 - length][2];

      uint8_t emptyIndex = 0;
      for (uint8_t i = 0; i < 8; i++) { // iterate thru all cells, note which are off
        for (uint8_t j = 0; j < 8; j++) {
          if (!ledStatus[i][j]) {
            emptyCells[emptyIndex][0] = i;
            emptyCells[emptyIndex][1] = j;
            emptyIndex++;
          }
        }
      }

      uint8_t randCell = (uint8_t) random(64 - length);
      locApple[0] = emptyCells[randCell][0];
      locApple[1] = emptyCells[randCell][1];
    }
  
  public:
    bool isGameOver;

    /** Snake constructor. Specify speed in dots per second. */
    Snake(uint8_t speed) {
      msPerDot = (unsigned int) round(1000.0 / speed);
    }

    /** Resets the game to start state. */
    void reset() {
      snakeHead[0] = snakeHead[1] = 3; // spawnpoint (1, 3)

      for (uint8_t i = 0; i < 64; i++) {
        snakeBody[i][0] = -1;
        snakeBody[i][1] = -1;
      }
      length = 3;
      dir = RIGHT;

      locApple[0] = 3; // first ever location (3, 6)
      locApple[1] = 6;

      isGameOver = false;
    }

    /** Intro sequence post-load. THIS IS A BLOCKING FUNCTION. */
    void onIntro() {
      snakeBody[0][0] = snakeHead[0]; // dumb manual initial propagation
      snakeBody[0][1] = snakeHead[1];
      snakeBody[1][0] = snakeHead[0];
      snakeBody[1][1] = snakeHead[1] - 1;
      snakeBody[2][0] = snakeHead[0];
      snakeBody[2][1] = snakeHead[1] - 2;

      render();
      setLedUpright(locApple[0], locApple[1], true);

      playStartSound();
    }

    /** Game over sequence. THIS IS A BLOCKING FUNCTION. */
    void onGameOver() {
      playFailSound();
      clearMatrix(); // clear screen
      delay(500);
    }

    /** Game loop. Call every iteration. Takes in last time delta in ms. */
    void loop(unsigned int delta) {
      // apple render stack

      shouldAppleMove = false;
      setLedUpright(locApple[0], locApple[1], true);

      // snake heading change stack

      if (dir == DOWN || dir == UP) { // only allow horizontal change when going vertically
        if (buttonStatus[0]) {
          dir = LEFT;
          tone(PIN_PZO, 311, 50); // based on Google's snake game
        }
        if (buttonStatus[1]) {
          dir = RIGHT;
          tone(PIN_PZO, 370, 50);
        }
      }

      if (dir == LEFT || dir == RIGHT) { // only allow vertical change when going horizontally
        if (buttonStatus[3]) {
          dir = DOWN;
          tone(PIN_PZO, 277, 50);
        }
        if (buttonStatus[4]) {
          dir = UP;
          tone(PIN_PZO, 415, 50);
        }
      }

      // snake move and render stack

      shouldSnakeMove = false;

      snakeMoveTimer += delta;
      if (snakeMoveTimer >= msPerDot) {
        shouldSnakeMove = true;
        snakeMoveTimer -= msPerDot;
      }

      if (shouldSnakeMove) {
        // move snake head
        switch (dir) {
          case UP:
            snakeHead[0] -= 1;
            break;
          case DOWN:
            snakeHead[0] += 1;
            break;
          case LEFT:
            snakeHead[1] -= 1;
            break;
          case RIGHT:
            snakeHead[1] += 1;
            break;
        }

        // snake eat or game over substack

        if (snakeHead[0] == locApple[0] && snakeHead[1] == locApple[1]) { // apple eat
          shouldAppleMove = true;
          length += 1;

          tone(PIN_PZO, 1480, 50);
        }
        else if (
          ledStatus[snakeHead[0]][snakeHead[1]] || // new head not at apple but at an on LED, which means new head inside snake body
          snakeHead[0] < 0 || snakeHead[0] > 7 || // new head out of bounds from any edge
          snakeHead[1] < 0 || snakeHead[1] > 7
        ) {
          isGameOver = true;
          return;
        }

        // substack end

        render(true); // clear current
        propagate();
      }

      render(); // at this point the apple is now part of the snake; exactly LENGTH number of LEDs are on

      // apple relocation stack

      if (shouldAppleMove) relocateRandom();
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

  // led matrix
  lc.shutdown(0, false);
  lc.setIntensity(0, 1); // holy f*ck why is this thing so bright

  // init
  clearMatrix();
  digitalWrite(PIN_LED, HIGH);
}

void loop() {
  // game over pre-signal input stack

  switch (nowGameID) {
    case 1:
      if (gamePong.isGameOver) gamePong.onGameOver();
      break;
    case 2:
      if (gameSnake.isGameOver) gameSnake.onGameOver();
      break;
  }

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

  // game load and unload stack

  if (Serial.available()) {
    uint8_t nextSig = Serial.parseInt();

    switch (nextSig) { // condition thru all possible signals
      case SIG_STOP_GAME:
        nowGameID = 0;
        clearMatrix();
        digitalWrite(PIN_LED, HIGH);
        break;
      case SIG_GAMEID_1: // pong
        nowGameID = 1;
        digitalWrite(PIN_LED, LOW);
        gamePong.reset();
        gamePong.onIntro();
        lastMillis = millis();
        break;
      case SIG_GAMEID_2: // snake
        nowGameID = 2;
        digitalWrite(PIN_LED, LOW);
        gameSnake.reset();
        gameSnake.onIntro();
        lastMillis = millis();
        break;
    }
  }

  // game over post-signal input stack

  switch (nowGameID) {
    case 1:
      if (gamePong.isGameOver) {
        gamePong.reset();
        gamePong.onIntro();
        lastMillis = millis();
      }
      break;
    case 2:
      if (gameSnake.isGameOver) {
        gameSnake.reset();
        gameSnake.onIntro();
        lastMillis = millis();
      }
      break;
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
