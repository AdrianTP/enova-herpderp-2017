#include "Arduino.h"

#include "headers/config.h"

#include "headers/game.h"
#include "headers/scanner.h"
#include "headers/lights.h"
#include "headers/sound.h"
#include "headers/button.h"

Game game;

void setup() {
  game.setup();
}

void loop() {
  game.tick();
}
