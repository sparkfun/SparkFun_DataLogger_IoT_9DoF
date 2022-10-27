// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition#
// 240MHz (WiFi/BT)
// Flash: 80MHz QIO 4MB "Default 4MB with SPIFFS"
// Core Debug: None
// PSRAM: Disabled
// Arduino Runs On: Core 1
// Events Run On: Core 1

// Deep Sleep Current Draw:
// OpenLog ESP32 is powered from an Otii Arc: https://www.sparkfun.com/products/18585
// Otii Arc is injecting power into the Li-Po connector. Voltage is set to 4.2V to mimic Li-Po
// After code upload, the USB-C cable is disconnected to avoid having the CH340 powered on
// Deep Sleep Current Draw is measured by the Otii Arc using Auto-Range
// Otii Arc is powered by a 9V wall wart and is calibrated before use

// Sleep current should be in the region of 200uA

#define STAT_LED 25 // OpenLog ESP32 STAT LED is connected to D25

#define IMU_CS 5
#define MAG_CS 27

#include "esp_sleep.h"

#include <SPI.h>

#include "SparkFun_ISM330DHCX.h" //Click here to get the library: http://librarymanager/All#SparkFun_6DoF_ISM330DHCX
SparkFun_ISM330DHCX_SPI myISM; // SPI instance class call

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA
SFE_MMC5983MA myMag;

void setup()
{
  pinMode(STAT_LED, OUTPUT); digitalWrite(STAT_LED, HIGH); // Turn the STAT LED on

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Disable the IMU accelerometers and gyros

  pinMode(IMU_CS, OUTPUT); digitalWrite(IMU_CS, HIGH);
  pinMode(MAG_CS, OUTPUT); digitalWrite(MAG_CS, HIGH);

  SPI.begin();

  delay(100);

  while( !myISM.begin(IMU_CS) )
  {
    //while(1)
    {
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(1800);
    }
  }

  // Reset the device to default settings
  // This if helpful is you're doing multiple uploads testing different settings. 
  myISM.deviceReset();

  // Wait for it to finish reseting
  while( !myISM.getDeviceReset() )
  { 
    {
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(1400);
    }
  } 

  delay(100);

  myISM.setAccelDataRate(ISM_XL_ODR_OFF); // Disable the accelerometers
  myISM.setAccelDataRate(ISM_GY_ODR_OFF); // Disable the gyros

  delay(100);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Disable the magnetometer

  SPISettings mySettings = SPISettings(3000000, MSBFIRST, SPI_MODE0);
  while ( !myMag.begin(MAG_CS, mySettings, SPI) ) // .begin sets up the CS pin as an OUTPUT
  {
    //while (1)
    {
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, HIGH); // Blink the STAT LED
      delay(200);
      digitalWrite(STAT_LED, LOW); // Blink the STAT LED
      delay(1000);
    }
  }

  myMag.softReset();

  delay(100);

  myMag.disableContinuousMode();
  
  delay(100);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Configure Sleep PowerDown

  SPI.end();

  esp_sleep_config_gpio_isolate();

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF);

  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Deep Sleep

  //digitalWrite(STAT_LED, LOW); // Turn the STAT LED off. Redundant since the pin is isolated during sleep.

  esp_sleep_enable_timer_wakeup(5000000ULL); // Sleep for 5 seconds - this will undo some of the pd_config settings

  esp_deep_sleep_start();
}

void loop()
{
  // Nothing to do here
}
