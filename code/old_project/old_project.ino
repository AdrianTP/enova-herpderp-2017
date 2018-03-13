#include <Servo.h>
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 12
#define SFX_RX 11

// Connect to the RST pin on the Sound Board
#define SFX_RST 10

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third 
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);

// Hello
const int lightsOnly = 0;

const int IDLE_STATE = 0;
const int PLAY_STATE = 1;
const int WIN_STATE =  2;
const int LOSE_STATE = 3;

const int buttonPin =   6;
const int numLedPins =  4;
const int numLeds =    12;
const int winLed =      6;

const int numPlaySounds = 2;
const int numWinSounds =  18;
const int numLoseSounds = 28;
const int numIdleSounds = 20;

const int pins[numLedPins] = {2, 3, 4, 5}; // Simulator buses: pink, purple, turquoise, orange

const int ledStates[][numLedPins] = {
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
  {2, 2, 1, 0}
};

// Begin Servo and Sensor Stuff
const int triggerPin    =   7;
const int echoPin       =   8; 
Servo rightArm;
int rightArmPos;
int sweepPos;
long duration;
int distance;
int armPin              =   9;
int winInterval         = 100;
unsigned long winMillis =   0;
int cwInterval          =  10;
int ccwInterval         =  20;
unsigned long armMillis =   0;
int sweepDirection      =   1;
// End Servo and Sensor Stuff

int previousButtonState = 0;
int buttonPressed       = 0;
int currentLed          = 0;
int scanDirection       = 0;
//int soundPlaying        = 0;
int playPlayed          = 0;
int sweepPlayed         = 0;
int idlePlayed          = 0;
int winPlayed           = 0;
int losePlayed          = 0;

unsigned long scanMillis =    0;
int scanInterval         =  100; //1000;

unsigned long blinkMillis =    0;
int blinkInterval         = 1000;
int blinkState            =    0;

unsigned long idleStateMillis =     0;
int idleStateLength           = 30000; // 30 seconds, length of idle loop
unsigned long playStartMillis =     0;
int playStateLength           = 10000; // 10 seconds, length of play state before auto-lose
unsigned long gameStateMillis =     0;
int gameStateInterval         =  5000; // 10 seconds, length of win/lose states before reset
int minGameStateInterval      =  1000;
int gameState                 =     0;

int useSerial  = 1;
int serialInit = 0;

int gameInit  = 0;
int firstBoot = 1;

int winDistance = 30;

void initSerial()
{
  if (useSerial && !serialInit)
  {
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.println("Serial port initialised");

    serialInit = 1;

    if (!lightsOnly)
    {
      Serial.println("Adafruit Sound Board!");
      
      // softwareserial at 9600 baud
      ss.begin(9600);
      // can also do Serial1.begin(9600)
    
      if (!sfx.reset()) {
        Serial.println("Not found");
        while (1);
      }
      Serial.println("SFX board found");
    }
  }
}

void setGameState(int target)
{
  unsigned long now = millis();

  if (now - gameStateMillis > minGameStateInterval)
  {
    gameState = target;
    gameStateMillis = now;

    if (useSerial && serialInit)
    {
      Serial.print("Game State: ");
      Serial.println(target);
    }

    playStartMillis = now;
    gameStateMillis = now;
  }
}

void initVars()
{
  buttonPressed =      0;
  currentLed    = winLed;
  scanDirection =      1;
  blinkState    =      0;
//  soundPlaying  =      0;
  playPlayed    =      0;
  sweepPlayed   =      0;
  idlePlayed    =      0;
  winPlayed     =      0;
  losePlayed    =      0;
  rightArmPos   =     93;
  sweepPos      =      0;
}

void lightsOff()
{
  for (int i = 0; i < numLedPins; i ++)
  {
    int pin = pins[i];

    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void initServo()
{
  rightArm.attach(armPin);
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void setup()
{
  if (gameInit == 0)
  {
    randomSeed(analogRead(A0));
    initSerial();
    initVars();
    setGameState(IDLE_STATE);

    pinMode(buttonPin, INPUT_PULLUP);
    digitalWrite(buttonPin, HIGH);

    lightsOff();

    initServo();

    gameInit = 1;
  }
}

void lightLed(int ledNum)
{
  const int* ledPinMap = ledStates[ledNum - 1];

  for (int i = 0; i < numLedPins; i ++)
  {
    int pin = pins[i];
    int state = ledPinMap[i];

    digitalWrite(pin, LOW);

    if (state == 2)
    {
      pinMode(pin, INPUT);
    }
    else
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, state);
    }
  }
}

int trackNumber()
{
  int numOfType;
  int number;

  switch (gameState)
  {
    case IDLE_STATE:
      numOfType = numIdleSounds;
      break;
    case PLAY_STATE:
      numOfType = numPlaySounds;
      break;
    case WIN_STATE:
      numOfType = numWinSounds;
      break;
    case LOSE_STATE:
      numOfType = numLoseSounds;
      break;
  }

  number = random(0, numOfType);

  return number;
}

char padding(int numSpaces)
{
  int padSize = 8 - numSpaces;
  char *spaces = malloc(padSize);
  for (int i = 0; i < padSize; i ++)
  {
    char tempSpaces = spaces;
    sprintf(tempSpaces, "%s%s", spaces, " ");
    spaces = tempSpaces;
  }

  return spaces;
}

String makeName(char type[5])
{
  char ext[5] = "WAV ";
  char trackNum[3];
  sprintf(trackNum, "%02d", trackNumber());
  char bob[7];
  sprintf(bob, "%s%s", type, trackNum);
  char fileName[50];
  sprintf(fileName, "%-8s%s", bob, ext);

  return String(fileName);
}

String trackName(int soundType)
{
  Serial.print("sound: ");
  Serial.println(soundType);
  
  switch (soundType)
  {
    case PLAY_STATE:
      return makeName("PLAY");
      break;
    case WIN_STATE:
      return makeName("WIN");
      break;
    case LOSE_STATE:
      return makeName("LOSE");
      break;
    case IDLE_STATE:
      return makeName("IDLE");
      break;
  }
}

void playSound(int soundType)
{
  if (!lightsOnly)
  {
    if (!sfx.playTrack((char*)trackName(soundType).c_str()))
    {
      Serial.println("Failed to play track?");
    }
  }
}

void scan()
{
  lightLed(currentLed);

  currentLed += scanDirection;

  if (currentLed == 1 || currentLed == numLeds)
  {
    scanDirection *= -1;
  }
}

boolean checkArmLimit()
{
  if (sweepDirection > 0)
  {
    return sweepPos <= 131;
  }
  else
  {
    return sweepPos >= 94;
  }
}

void updateArmPosition(unsigned long now)
{
  sweepPos = rightArmPos;
  armMillis = now;

  sweepPos += sweepDirection;

  rightArm.write(sweepPos);
  rightArmPos = sweepPos;

  if (!checkArmLimit()) sweepDirection *= -1;
}

void sweep()
{
  unsigned long now = millis();
  int localInterval = sweepDirection > 0 ? cwInterval : ccwInterval;

  if (now - scanMillis >= scanInterval)
  {
    scanMillis = now;

    scan();
  }
  
  if (!sweepPlayed)
  {
    sweepPlayed = 1;
    playSound(PLAY_STATE);
  }
  
  if (now - armMillis >= localInterval)
  {
    updateArmPosition(now);
    armMillis = now;
  }
  
  checkWin();

//  Serial.print("now: ");
//  Serial.print(now);
//  Serial.print(" start: ");
//  Serial.println(playStartMillis);

  if (now - playStartMillis >= playStateLength)
  {
    Serial.println("switching to lose");
    setGameState(LOSE_STATE);
    gameStateMillis = now;
  }
}

//void play()
//{
//  unsigned long now = millis();
//
//  if (now - scanMillis >= scanInterval)
//  {
//    scanMillis = now;
//
//    scan();
//  }

//  if (!playPlayed)
//  {
//    playPlayed = 1;
//    playSound(PLAY_STATE);
//  }
//}

void blink()
{
  unsigned long now = millis();

  if (now - blinkMillis >= blinkInterval)
  {
    blinkMillis = now;

    if (blinkState)
    {
//      lightLed(currentLed);
      
      for (int i = 0; i < numLeds; i ++)
      {
        lightLed(i + 1);
      }
    }
    else
    {
      lightsOff();
    }

    blinkState = !blinkState;
  }
}

void win()
{
  unsigned long now = millis();
  
  blink();

  if (!winPlayed)
  {
    winPlayed = 1;
    playSound(WIN_STATE);
  }

  if (now - gameStateMillis >= gameStateInterval)
  {
    gameInit = 0;
    setGameState(IDLE_STATE);
  }
}

void lose()
{
  unsigned long now = millis();
  
  blink();

  if (!losePlayed)
  {
    losePlayed = 1;
    playSound(LOSE_STATE);
  }

  if (now - gameStateMillis >= gameStateInterval)
  {
    gameInit = 0;
    setGameState(IDLE_STATE);
  }
}

void idle()
{
  unsigned long now = millis();

  blink();
  
  setup();

  if (!idlePlayed)
  {
    idlePlayed = 1;
    playSound(IDLE_STATE); // Music or speech loop "hook"
  }

  if (now - idleStateMillis >= idleStateLength)
  {
    idlePlayed = 0;
    idleStateMillis = now;
  }
}

int checkDistance()
{
  if (lightsOnly) return 100;
  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  return duration / 52;
}

void checkWin() {
  unsigned long now = millis();

  if (now - winMillis >= winInterval)
  {
    Serial.print("distance: ");
    Serial.println(checkDistance());
    
    if (checkDistance() >= winDistance) {
      delay(500);
      if (checkDistance() >= winDistance) setGameState(WIN_STATE);
    }

    winMillis = now;
  }
}

void checkButton()
{
  buttonPressed = digitalRead(buttonPin);
  unsigned long now = millis();

  if ((buttonPressed != previousButtonState) && buttonPressed)
  {
    if (gameState == IDLE_STATE && buttonPressed)
    {
      if (firstBoot)
      {
        setGameState(PLAY_STATE);
        firstBoot = 0;
      }
      else if (now - gameStateMillis >= gameStateInterval)
      {
        setGameState(PLAY_STATE);
      }
    }
  }

  previousButtonState = buttonPressed;

//  unsigned long now = millis();

//  if (gameState == PLAY_STATE && buttonPressed && currentLed == winLed)
//  {
//    setGameState(WIN_STATE);
//  }
//  else if(gameState == PLAY_STATE && buttonPressed && currentLed != winLed)
//  {
//    setGameState(LOSE_STATE);
//  }
//  else 
//  if (gameState == IDLE_STATE && buttonPressed)
//  {
//    if (firstBoot)
//    {
//      setGameState(PLAY_STATE);
//      firstBoot = 0;
//    }
//    else if (now - gameStateMillis >= gameStateInterval)
//    {
//      setGameState(PLAY_STATE);
//    }
//  }
}

//void lightGame()
//{
//  switch (gameState)
//  {
//  	case PLAY_STATE:
//      play();
//      break;
//    case WIN_STATE:
//      win();
//      break;
//    case LOSE_STATE:
//      lose();
//      break;
//    case IDLE_STATE:
//      idle();
//      break;
//  }
//
//  checkButton();
//
//  unsigned long now = millis();
//
//  if ((gameState == WIN_STATE || gameState == LOSE_STATE) && now - gameStateMillis >= gameStateInterval)
//  {
//    gameInit = 0;
//    setGameState(IDLE_STATE);
//  }
//}

void sensorGame()
{
  unsigned long now = millis();
  
  switch (gameState)
  {
    case PLAY_STATE:
      sweep();
      break;
    case WIN_STATE:
      win();
      break;
    case LOSE_STATE:
      lose();
      break;
    case IDLE_STATE:
      idle();
      break;
  }

  checkButton();
}

void loop()
{
  sensorGame();
}
