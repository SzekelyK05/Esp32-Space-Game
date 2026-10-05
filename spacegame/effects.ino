//spacegame
void playLaserSound() {
  for (int freq = 3000; freq >= 1000; freq -= 100) {
    ledcWriteTone(buzzer, freq);
    delay(1);
  }
  ledcWriteTone(buzzer, 0);
}

void playHitSound() {
  for (int freq = 800; freq >= 100; freq -= 100) {
    ledcWriteTone(buzzer, freq);
    delay(2); 
  }
  ledcWriteTone(buzzer, 0);
}

void playGameOverSound() {
  for (int freq = 600; freq >= 100; freq -= 10) {
    ledcWriteTone(buzzer, freq);
    delay(15); 
  }
  ledcWriteTone(buzzer, 0);
}

void updateStars() {
  for(int i=0; i<num_stars; i++)
  {
    starX[i]-=1;
    if (starX[i] < 0) {
      starX[i] = SCREEN_WIDTH;
      starY[i] = random(16, SCREEN_HEIGHT);
    }
    display.drawPixel(starX[i], starY[i], SSD1306_WHITE);
  }

}
