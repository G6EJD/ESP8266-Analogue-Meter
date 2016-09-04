// (C) D Bird 2016
// TFT version of an analogue meter, reads the anologue input pin and display the resultant value on the 'meter'

#include <SPI.h>
#include <SD.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

//----------------------------------------------------------------------------------------------------
#// For the Adafruit shield, these are the default.
#define SD_CS  D3     // Chip Select for SD-Card
#define TFT_DC D4     // Data/Command pin for SPI TFT screen
#define TFT_CS D8     // Chip select for TFT screen

#define number_of_readings 100 // Number of readings to hold in an array
#define major_graduations 0.1  // number_of_readings / 10 to set the scale graduations
#define delay_val 200          // Meter refresh rate in mS

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

float start_angle = -2.5, end_angle = -0.62, last_value = -1, input_value = 0.00;
int x = 155, y = 210, r = 150, scale = 0; // Meter needle X,Y coordinates plus radius all in pixels, values are for a 320x240 screen

void display_meter(float scale){
  //tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);   // Set coursor to the top left for the title
  tft.setTextSize(2);   // Set text size to normal for the ILI9341
  tft.println("TFT Analogue Meter Example"); // Display the title

  // Now draw the meter using a two pixel line to emphasise it
  for (float i = start_angle; i < end_angle; i = i + 0.01)
  {
    tft.drawPixel(x + cos(i) * r, y + sin(i) * r, ILI9341_WHITE); // center point is (x,y)
    tft.drawPixel(x + cos(i) * r * 1.01, y + sin(i) * r * 1.01, ILI9341_WHITE); 
  }
  //  0 = start_angle
  // 10 = end_angle
  // per-unit step = start_angle + (end_angle-start_angle)*major_graduations) * value to be displayed
  // Now draw the graduations and scale
  for (float i = start_angle; i < end_angle; i = i + (end_angle-start_angle)*major_graduations) 
  {
    tft.drawLine(x + cos(i) * r, y + sin(i) * r, x + cos(i) * r * 1.1, y + sin(i) * r * 1.1, ILI9341_WHITE); 
    tft.setCursor(x + cos(i) * r * 1.16-3, y + sin(i) * r * 1.16);
    tft.setTextSize(1);
    tft.println(scale,0);
    scale++;
  }
  tft.drawCircle(x,y,5, ILI9341_GREEN); // Draw a small circel around the meter needle hinge point
  tft.drawRoundRect(2,22, 310, 200, 15, ILI9341_GREEN); // Draw the outer rectangle
}

// Draw the arrow head
void arrow(int x1, int y1, int x2, int y2, int alength, int awidth, int colour) {
  float distance;
  int dx,dy,x2o,y2o,x3,y3,x4,y4,k;
  distance = sqrt(pow((x1 - x2),2) + pow((y1 - y2), 2));
  dx = x2 + (x1 - x2) * alength / distance;
  dy = y2 + (y1 - y2) * alength / distance;
  k = awidth / alength;
  x2o = x2 - dx;
  y2o = dy - y2;
  x3 = y2o * k + dx;
  y3 = x2o * k + dy;
  //
  x4 = dx - y2o * k;
  y4 = dy - x2o * k;
  tft.drawLine(x1, y1, x2, y2, colour);
  tft.drawLine(x1, y1, dx, dy, colour);
  tft.drawLine(x3, y3, x4, y4, colour);
  tft.drawLine(x3, y3, x2, y2, colour);
  tft.drawLine(x2, y2, x4, y4, colour);
} 

// Draw the meter needle and add the arrow head
void display_value (float value){
  int x2,y2;
  x2 = x + cos(start_angle+(end_angle-start_angle)*major_graduations*last_value) * r * 0.98;
  y2 = y + sin(start_angle+(end_angle-start_angle)*major_graduations*last_value) * r * 0.98;
  if (last_value != value) {
    arrow(x,y,x2, y2, 10, 10, ILI9341_BLACK);   // Erase arrow in old position
    tft.drawLine(x, y, x2, y2, ILI9341_BLACK);
  }
  x2 = x + cos(start_angle+(end_angle-start_angle)*major_graduations*value) * r * 0.98;
  y2 = y + sin(start_angle+(end_angle-start_angle)*major_graduations*value) * r * 0.98;
  tft.drawLine(x, y, x2, y2, ILI9341_GREEN);
  arrow(x, y, x2, y2, 10, 10, ILI9341_GREEN);    // Draw arrow in new position
  last_value = value;
  tft.drawCircle(x,y,5, ILI9341_GREEN);
}

//----------------------------------------------------------------------------------------------------

void setup(){
  Serial.begin(115200);
  analogWriteFreq(500); // Enable TFT display brightness // Used on the Adafruit dislay to reduce screen brightness
  analogWrite(D0, 750); // Set display brightness using D0 as driver to TFT LED // D0 is connected to the LED pin on the TFT
  pinMode(A0, INPUT);   // The pin used for the analogue input to be displayed
  tft.begin();          // Start TFT display support
  tft.setRotation(3);   // Rotate through 270-degrees
  tft.setTextSize(2);   // Set the default text size, until changed!
  tft.fillScreen(ILI9341_BLACK); // Clear the screen
}
// ---------------------------------------------------

void loop(){
  display_meter(0);                // Draw the meter
  input_value = analogRead(A0);    // Read an analogue input pin the result is 0 - 1023
  input_value = input_value / 100; // scales reading from 0-1023 to 0-102.3, in effect 0-100
  display_value(input_value);      // Undraw then Draw the meter needle
  delay(delay_val);                // Wait a short while
}  


