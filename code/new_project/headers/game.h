/*
 * =====================================================================================
 *
 *       Filename:  game.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/19/2017 16:48:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef GAME_H
#define GAME_H

class Scanner;
class Lights;
class Sound;
class Button;

class Game {
  private:
    // Class instances
    Scanner* scanner;
    Lights* lights;
    Sound* sound;
    Button* button;

    // Vars
    unsigned long previousStateMillis;
    int stateInterval;
    int minStateInterval;
    int state;
    int initialized;
    int firstBoot;

  public:
    // Constructor
    Game();

    // Destructor
    virtual ~Game();

    void setup();

    void setState(int targetState);

    void play();

    void win();

    void lose();

    void idle();

    void tick();
}

#endif
