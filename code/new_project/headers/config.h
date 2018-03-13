#ifndef CONFIG_H
#define CONFIG_H

const bool SERIAL_DEBUG = true;
const int SERIAL_DEBUG_INTERVAL = 1000;

const bool RANDOM_SEED_DEBUG = true;
const int RANDOM_SEED_VALUE = 100;

const int BUTTON_PIN = 6;
const int NUM_LED_PINS = 4;
const int NUM_LEDS = 11;
const int WIN_LED = 6;

const int NUM_PLAY_SOUNDS = 1;
const int NUM_WIN_SOUNDS = 5;
const int NUM_LOSE_SOUNDS = 5;
const int NUM_IDLE_SOUNDS = 1;

const int PINS[NUM_LED_PINS] = {2, 3, 4, 5};

const int LED_STATES[][NNUM_LED_PINS] = {
  {0, 1, 2, 2},
  {1, 0, 2, 2},
  {0, 2, 1, 2},
  {1, 2, 0, 2},
  {0, 2, 2, 1},
  {1, 2, 2, 0},
  {2, 0, 1, 2},
  {2, 1, 0, 2},
  {2, 0, 2, 1},
  {2, 1, 2, 0},
  {2, 2, 0, 1},
  {2, 2, 2, 2}
};

namespace GameState {
  enum GameState {
    IDLE,
    PLAY,
    WIN,
    LOSE
  };
};

const int STATE_INIT = 0;

#endif
