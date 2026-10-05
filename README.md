# Esp32-Space-Game

## Bill of Materials
The project needs an esp32 (ESP32-WROOM-32), 0.96 inch oled display,  HW-508 passive buzzer, 2 tactile switch buttons, breadboard and jumper wires.
For the hand input mode, a HC-SR04 ultrasonic sensor.
No soldering needed.


## Wiring
Passive Buzzer:     - -> GND
                    + -> Pin 4


Oled display:       SCL -> Pin 22
                    SDA -> Pin 21
                    GND -> GND
                    VCC -> 3V3

### Button control
UP button:          Left leg -> Pin 18
                    Right leg -> GND

DOWN button:        Left leg -> Pin 19
                    Right leg -> GND

### Hand control
Ultrasonic sensor:  Vcc -> VIN
                    Trig -> Pin 18
                    Echo -> Pin 19
                    Gnd -> Gnd


## Dependencies
Adafruit_GFX, Adafruit_SSD1306, Wire

## User guide
By giving power to the Esp-32, the menu of the game will appear. Here the high score is visible.

After pressing any of the 2 buttons(UP/DOWN), the game will start. 
The player is controlling a spaceship and shoots lasers automatically. On the right, 2 types enemies are spawning(Alien, Ufo) and are aproaching the left side.
The objective is to shoot the enemies before they reach the left end of the screen. After shooting any enemy, the score will increase.
If an enemy reaches the left end, the game will end and the menu will appear again.

## Code features
After every fifth score, the pace of the game increases (and decreases after every 20th).
Random spawning point for the enemies on the y-axis.
Special game sounds made using frequency sweepes.
Background stars made by randomly spawning pixels on the screen.

## Hand input mode
Insted of buttons, the spaceship can be controlled using your hand.
This can be done by orienting the ultrasonic sensor to point up and placing your hand above it. Range: 10cm - 30cm.
Note: The hand input version uses an earlier version of the code (that only has one type of enemy instead of 2). This ensures more approachable gameplay.
The "spacegame_hand" folder contains the code for this version.

## Installation

### Prerequisites
1. Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the ESP32 board package in the Arduino IDE (follow [Espressif's official guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) if you haven't done this before).

### Setup Steps
1. **Clone the repository:**
   ```bash
   git clone (https://github.com/SzekelyK05/Desktop-buddy.git)

(Or download the repository as a ZIP file and extract it).
2. Open the sketch: Open the src/main.ino file in the Arduino IDE.
3. Install dependencies: Go to Tools > Manage Libraries... in the Arduino IDE, search for and install the following:

        Adafruit GFX Library
        Adafruit SSD1306

Configure your board:

Go to Tools > Board and select ESP32 Dev Module (or your specific ESP32 variant).

Go to Tools > Port and select the USB port your ESP32 is connected to.

Flash the firmware: Click the Upload button to compile and push the code to your board.

