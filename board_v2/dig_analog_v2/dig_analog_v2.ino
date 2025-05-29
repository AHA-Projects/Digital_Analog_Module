// Acknowledgments

// Creator: Anany Sharma at the University of Florida working under NSF grant. 2405373

// This material is based upon work supported by the National Science Foundation under Grant No. 2405373.
// Any opinions, findings, and conclusions or recommendations expressed in this material are those of the authors and do not necessarily reflect the views of the National Science Foundation.

#include <Wire.h> 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>             // Required for SPI communication
#include <math.h>            // Needed for plotting the sin graph

// --- Display Pin Definitions ---
#define TFT_CS   33  // Chip Select control pin
#define TFT_DC    25  // Data/Command select pin
#define TFT_RST   26  // Reset pin (or connect to RST, see below)


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// --- Display Constants (Adjusted for 320x170 landscape) ---
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 170

// Input Pins (Assuming the same pins for ESP32)
#define POT_PIN 32       // Analog input for potentiometer
#define LEFT_SWITCH_PIN 36 // Digital input for left switch
#define RIGHT_SWITCH_PIN 39 // Digital input for right switch

#define ADC_MAX 4095.0

// --- Global Variable ---
// Define the Y coordinate for the base digital level (both switches OFF)
// Let's place it about 30 pixels from the bottom for better visibility on a taller screen.
const int BASE_Y_LEVEL = SCREEN_HEIGHT - 30;

// --- Colors (Using 16-bit color values) ---
#define BLACK   0x0000
#define WHITE   0xFFFF
#define BLUE    0x001F
#define GREEN   0x07E0
#define RED     0xF800

void setup() {
  Serial.begin(115200);

  // --- Display Initialization ---
  tft.init(SCREEN_HEIGHT, SCREEN_WIDTH); // Initialize ST7789 with your resolution
  tft.setRotation(3); 
  tft.fillScreen(BLACK); // Clear the screen with black

  // Initialize inputs
  pinMode(POT_PIN, INPUT);
  pinMode(LEFT_SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor
  pinMode(RIGHT_SWITCH_PIN, INPUT_PULLUP); // Use internal pull-up resistor

  tft.setCursor(0, 0);       // Set cursor to top-left
  tft.setTextColor(WHITE); // Set text color to white
  tft.setTextSize(2);        // Set text size
  tft.println("Analog/Digital Demo");

  delay(1000); // Display initial message for a moment
}

// Function to read the potentiometer value and map it to a desired range
float readPotentiometer(float minVal, float maxVal) {
  float sensorValue = analogRead(POT_PIN);
   float mappedValue = minVal + (sensorValue / ADC_MAX) * (maxVal - minVal);
  return mappedValue;
}

// Function to draw a sinusoidal wave centered at a specific Y coordinate
// Amplitude here is the peak deviation from the center Y
void drawSineWave(float amplitude, float frequency, int centerY) {
  int maxY = SCREEN_HEIGHT - 1;
  int minY_analog_area = 40; // Leave space for text at the top

  if (centerY + amplitude >= maxY) amplitude = maxY - 1 - centerY;
  if (centerY - amplitude < minY_analog_area) amplitude = centerY - minY_analog_area;

  if (amplitude < 1) amplitude = 1; 
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    // Calculate the y-value for the sine wave relative to the center Y
    float yValue = amplitude * sin(2.0 * PI * frequency * (float)x / SCREEN_WIDTH) + centerY;

    // Draw the point if it's within screen bounds and the analog area
    if (yValue >= minY_analog_area && yValue < SCREEN_HEIGHT) {
      tft.drawPixel(x, (int)round(yValue), WHITE); // Draw the pixel in white
    }
  }
}

// Function to draw the digital step function line and associated text
void drawDigitalStep(bool leftState, bool rightState) {
  int level = 1;
  if (leftState && !rightState) level = 1;  // Left ON only
  if (!leftState && rightState) level = 2;  // Right ON only

  // Calculate the Y coordinate for the step function line based on level
  int yCoordinate;
  String levelText = "OFF";

  // Adjust Y coordinates for the new screen height
  switch (level) {
    case 1:
      yCoordinate = SCREEN_HEIGHT - 30; // Level 1 near the base
      levelText = "0";
      break;
    case 2:
      yCoordinate = SCREEN_HEIGHT - 70; // Level 2 higher
      levelText = "1";
      break;
  }

  tft.drawFastHLine(0, yCoordinate, SCREEN_WIDTH, GREEN);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Mode: Digital Step");

  tft.setCursor(0, 20); // Move down
  tft.setTextColor(WHITE);
  tft.print("L:");
  tft.print(leftState ? "ON " : "OFF");
  tft.print(" R:");
  tft.println(rightState ? "ON " : "OFF");

  tft.setCursor(120, 50); // Move down for the next line
  tft.print("LEVEL: ");
  tft.setTextColor(GREEN); // Color for the level text
  tft.println(levelText);



}

int previousDigitalY = BASE_Y_LEVEL;
float previousAmplitude = 0; // Initialize to a value that won't draw anything significant initially
float previousFrequency = 0;


void loop() {
  // Read the switch states 
  bool leftSwitchState = !digitalRead(LEFT_SWITCH_PIN);
  bool rightSwitchState = !digitalRead(RIGHT_SWITCH_PIN);

  // Clear the display for the new frame
  tft.fillScreen(BLACK);

  // --- Conditional Logic: Decide whether to show Analog or Digital ---
  if (!leftSwitchState && !rightSwitchState) {
    // --- ANALOG MODE: No switches pressed ---

    // Read amplitude & frequency from potentiometer
    float max_analog_amplitude = BASE_Y_LEVEL - 40; // Leave space for top text
    if (max_analog_amplitude < 1) max_analog_amplitude = 1; // Ensure minimum amplitude range
    float amplitude = readPotentiometer(1.0, max_analog_amplitude); 
    float frequency = readPotentiometer(0.5, 10.0); // Adjusted frequency range for wider screen

    // Display Status Text for Analog Mode
    tft.setCursor(0, 0);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.println("Mode: Analog Wave");

    tft.setCursor(0, 20); 
    tft.print("Freq: ");
    tft.setTextColor(BLUE); 
    tft.print(frequency, 1); 
    tft.println(" cycles");


    drawSineWave(amplitude, frequency, BASE_Y_LEVEL);

  } else {
    drawDigitalStep(leftSwitchState, rightSwitchState);
  }
  delay(1);
}