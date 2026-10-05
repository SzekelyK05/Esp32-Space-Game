#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "graphics.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Not used with ESP32
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SOUND_VELOCITY 0.034
#define ECHO_TIMEOUT_US 25000UL   // ~4m max range; keeps pulseIn from blocking for a full second

#define trigPin 18
#define echoPin 19
#define buzzer 4

const int channel=0;

int shipY = 20;
int shotX;
int shotposY=26;
int obstacleX = SCREEN_WIDTH;
int obsposY;              // now assigned in setup(), after randomSeed()
int score=0;
int highscore=0;
int obsspeed=5;

//background effect variables
const int num_stars=15;
int starX[num_stars];
int starY[num_stars];

//ultrasonic sensor variables
long duration;
float distanceCm;
float prevDist = 20;      // sane default (mid-range) instead of 0, in case no hand is detected on boot
bool detected=false;

bool hitshot=false;

enum option {menu, game};
option State=menu;


void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  ledcAttachChannel(buzzer, 2000, 8, channel);
  Wire.begin(21, 22);

  randomSeed(analogRead(0));   // seed the RNG so obstacle positions aren't identical every boot
  obsposY = random(20, 56);    // moved here, after seeding, and range matches the in-game reset

  for(int i=0; i<num_stars; i++)
  {
    starX[i]=random(0, SCREEN_WIDTH);
    starY[i]=random(16, SCREEN_HEIGHT);
  }

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.display();
}

void loop() {

  handleUltrasonic();
  switch(State)
  {
    case menu:
      handleMenu();
    break;

    case game:
      handleGame();
    break;

    default: break;
  }

  
}

void handleMenu()
{
      if(score>highscore)
        highscore=score;
      display.clearDisplay();
      display.setCursor(32, 0);
      display.print("Space game");
      display.drawLine(0, 15, SCREEN_WIDTH, 15, SSD1306_WHITE);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(10, 25);
      display.print("Hover your hand!");
      display.setTextSize(1);
      display.setCursor(20, 45);
      display.print("High score: ");
      display.print(highscore);
      display.display();

      if(detected)
      {
        shipY = 20;
        score = 0;
        obstacleX = SCREEN_WIDTH;
        obsposY = random(20, 56);
        shotX = 10;
        hitshot = false;
        obsspeed=5;
        State=game;
        playLaserSound();
      }
}

void handleGame()
{
  //Handle ship movement with ultrasonic sensor
  int handY;
  if(detected)
  {
    prevDist=distanceCm;
    handY=map((long)distanceCm, 10, 30, 48, 14);
  }
  else
    handY=map((long)prevDist, 10, 30, 48, 14);

  handY = constrain(handY, 14, 48);   // guard against any extrapolation past the intended range

  if(shipY<handY)
    shipY+=2;
  else if(shipY>handY)
    shipY-=2;

  shipY = constrain(shipY, 14, 48);   // never let the ship drift off-screen

  
  //shot
  shotX += 5 + obsspeed/5;
  if (shotX > 138) {
    shotX = 10;
    shotposY = shipY + 6;
    hitshot = false;
    
    playLaserSound();
  }


  //obstacle
  obstacleX-=obsspeed/5;
  if (obstacleX < -10) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 25);
    display.print("Game Over");
    display.setTextSize(1);
    display.setCursor(34, 45);
    display.print("Score: ");
    display.print(score);
    display.display();
    playGameOverSound();
    delay(2000);
    State=menu;
    return;
  }

  //hit
  if (!hitshot) {
    if (shotX < obstacleX + 8 && 
        shotX + 10 > obstacleX && 
        shotposY < obsposY + 8 && 
        shotposY + 3 > obsposY) {
          
      hitshot = true;
      obstacleX = SCREEN_WIDTH;
      obsposY = random(20, 56);
      score++;
      obsspeed++;
      if(score%20==0)
        obsspeed/=2;
      playHitSound();
    }
  }
 


  //Clear Display
  display.clearDisplay();

  //background
  updateStars();
  
  //score
  display.setCursor(1, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Score: ");
  display.print(score);
  
  //Draw ship 
  display.drawBitmap(10, shipY, spaceship, 16, 16, SSD1306_WHITE);

  //Draw line
  display.drawLine(0, 15, SCREEN_WIDTH, 15, SSD1306_WHITE);

  // Draw shot
  if(!hitshot)
    display.fillRect(shotX, shotposY, 10, 3, SSD1306_WHITE);

  //Draw obstacle 
  display.drawBitmap(obstacleX, obsposY, alien, 8, 8, SSD1306_WHITE);
  
  display.display();
  delay(30);
}

void handleUltrasonic()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);  // bounded timeout - no more 1s stalls

  if (duration == 0) {
    // no echo received within the timeout window -> treat as "nothing in range"
    detected = false;
    return;
  }

  distanceCm = duration * SOUND_VELOCITY/2;

  // decide detection on the RAW reading, before any clamping
  detected = (distanceCm >= 10 && distanceCm <= 30);

  // only clamp the value actually used by map() in handleGame(), and only when it's in range
  if (detected)
    distanceCm = constrain(distanceCm, 10, 30);
}
