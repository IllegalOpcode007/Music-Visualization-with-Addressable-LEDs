/***** Include Libraries *****/
/***********************************/
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>
/***********************************/

#define LED_PIN 13 // LED Input
#define COLOR_ORDER GRB
#define TOT_NUM_LEDS 150 // For defining array size
#define NUM_LEDS 149 // For iterating through array
#define intensityThd 480 // Threshold for other sensor: 742
#define consecMaxThd 10 // Max consecutive amount of times to read in values greater than intensityThd

CRGB leds[TOT_NUM_LEDS]; // Set Number of LEDS in strip
CRGB ledBuffer[TOT_NUM_LEDS]; // Saves past state of LEDS

int sensorPin = A0; // Sensor Input
int soundSensorData[TOT_NUM_LEDS];
int sensorDataBuffer[TOT_NUM_LEDS]; // Saves past state of sound sensor value

// Initialize color to start with and color increment
int r = 255;
int g = 0;
int b = 0;
int colIncDec = 3;

// Initialize a few other vars
int sensorValue = 0;
int consecMax = 0;

void setup() 
{
  // initialize Serial Communication with Computer:
  Serial.begin(250000);
  // Inititialize Addressable LEDs
  FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, TOT_NUM_LEDS);
}

void loop()
{
  // Read current sensor value
  sensorValue =  analogRead(sensorPin);

  // Shift LED state by 1 and save in ledBuffer buffer
  for(int i=0; i<=NUM_LEDS-1; i++)
  {
    ledBuffer[1 + i] = leds[i]; // save past LED state in ledBuffer but shifted one up
    sensorDataBuffer[1+i] = soundSensorData[i]; // shift past sensor values up
  }

  // Copy ledBuffer buffer to output
  for(int i = 0; i<=NUM_LEDS; i++)
  {
    leds[i] = ledBuffer[i];
    soundSensorData[1+i] = sensorDataBuffer[i];
  }

  // Turn off First LED
  ledBuffer[0] = CRGB(0,0,0);
  leds[0] = CRGB(0,0,0);
  
  // Light first LED if greater than certain threshold and threshold has not been met for a certain consecutive amount of times to prevent a freezed state
  if (sensorValue > intensityThd && consecMax != consecMaxThd) 
  {
    FastLED.setBrightness(sensorValue); // Brightness will be based on sensor value
    // Gradually change colors as LED lights up
    if(r > 0 && b == 0)
    {
      r = r - colIncDec;
      g = g + colIncDec;
    }
    if(g > 0 && r == 0)
    {
      g = g - colIncDec;
      b = b + colIncDec;
    }
    if(b > 0 && g == 0)
    {
      r = r + colIncDec;
      b = b - colIncDec;
    }
    
    leds[0] = CRGB(r,g,b); // Illuminate and Color LED
    consecMax++; 
  }
  else
  {
    // Turn Off LED if Threshold not met
    FastLED.setBrightness(150);
    consecMax = 0;
    leds[0] = CRGB(0,0,0);
  }
  soundSensorData[0] = sensorValue; // Save Current Sensor Value

  FastLED.show(); // Output to physical LED
  delay(6); // Delay so LEDs don't shift too quickly.

}
