#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "graphics.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define UP 18
#define DOWN 19
#define buzzer 4

const int channel=0;

int shipY = 20;
int shotX;
int shotposY=26;
int obstacleX = SCREEN_WIDTH;
int obsposY=random(20, 56);
int obsspeed=5;
bool hitshot=false;

int alien2X = SCREEN_WIDTH + 64; 
int alien2Y = random(20, 56);
int alien2DirY = 1;
bool hitAlien2 = false;
int score=0;
int highscore=0;


//background effect variables
const int num_stars=15;
int starX[num_stars];
int starY[num_stars];


enum option {menu, game};
option State=menu;


void setup() {
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  ledcAttachChannel(buzzer, 2000, 8, channel);
  Wire.begin(21, 22);

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
      display.print("Press any button");
      display.setTextSize(1);
      display.setCursor(20, 45);
      display.print("High score: ");
      display.print(highscore);
      display.display();

      if(digitalRead(UP)==LOW || digitalRead(DOWN)==LOW)
      {
        shipY = 20;
        score = 0;
        obstacleX = SCREEN_WIDTH;
        obsposY = random(20, 56);
        hitshot = false;

        alien2X = SCREEN_WIDTH + 64; 
        alien2Y = random(20, 56);
        hitAlien2 = false;

        shotX = 10;
        obsspeed=5;
        State=game;
        playLaserSound();
      }
}

void handleGame()
{
  //Handle UP
  if (digitalRead(UP) == LOW) {
    if(shipY<18)
      shipY=18;
    shipY=shipY-3;
  }

  //Handle DOWN
  if (digitalRead(DOWN) == LOW) {
    if(shipY>48)
      shipY=48;
    shipY=shipY+3;
  }
  
  //shot
  shotX += 5 + obsspeed/5;
  if (shotX > 138) {
    shotX = 10;
    shotposY = shipY + 6;
    hitshot = false;
    
    playLaserSound();
  }


  //enemy movement
  obstacleX-=obsspeed/5;

  alien2X -= obsspeed/5;
  alien2Y += alien2DirY;
  if (alien2Y <= 18 || alien2Y >= 56) {
    alien2DirY = -alien2DirY;
  }

  //game over logic
  if (obstacleX < -10 || alien2X < -10) {
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

  //hit alien
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

  //hit ufo
  if (!hitshot) {
    if (shotX < alien2X + 8 && 
        shotX + 10 > alien2X && 
        shotposY < alien2Y + 8 && 
        shotposY + 3 > alien2Y) {
          
      alien2X = SCREEN_WIDTH + random(20, 50);
      alien2Y = random(20, 56);
      score++;
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

  //Draw enemies 
  display.drawBitmap(obstacleX, obsposY, alien, 8, 8, SSD1306_WHITE);
  display.drawBitmap(alien2X, alien2Y, ufo, 8, 8, SSD1306_WHITE);
  
  display.display();
  delay(30);
}
