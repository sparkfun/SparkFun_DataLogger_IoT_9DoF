// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

#define EN_3V3_SW 32 // The 3.3V_SW regulator Enable pin is connected to D32
#define RGB_LED 26 // OpenLog ESP32 RGB LED is connected to D26

#include <FastLED.h> // Click here to get the library: http://librarymanager/All#FastLED

// Note: With FastLED v3.5.0: Arduino\libraries\FastLED\src\platforms\esp\32\clockless_rmt_esp32.cpp generates two unused variable errors:
// "Arduino\libraries\FastLED\src\platforms\esp\32\clockless_rmt_esp32.cpp:111:15: error: variable 'espErr' set but not used [-Werror=unused-but-set-variable]"
// You can fix this by adding (void)espErr; at line 112 and again at line 241

#define COLOR_ORDER GRB
#define CHIPSET     WS2812
#define NUM_LEDS    1

#define BRIGHTNESS  50

CRGB leds[NUM_LEDS];

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void setup() {
  pinMode(EN_3V3_SW, OUTPUT);
  digitalWrite(EN_3V3_SW, HIGH);
  
  delay(30); // sanity delay
  
  FastLED.addLeds<CHIPSET, RGB_LED, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

void loop()
{
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
  
  EVERY_N_MILLISECONDS( 10 ) { gHue++; } // cycle the "base color" through the rainbow
}
