// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition
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

// Step 1: Call esp_deep_sleep_start(); and nothing else
// Step 2: Disable Wake-Up Sources (ALL) and Configure Sleep PowerDown (set _all_ domains to OFF)
// Step 3: Initialize the ISM330 IMU and disable the accelerometers and gyros
// Step 4: Initialize the MMC5983MA Mag and call myMag.disableContinuousMode();

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

  // Step 3: Disable the IMU accelerometers and gyros

  pinMode(IMU_CS, OUTPUT); digitalWrite(IMU_CS, HIGH);
  pinMode(MAG_CS, OUTPUT); digitalWrite(MAG_CS, HIGH);

  SPI.begin();

  delay(100);

  if( !myISM.begin(IMU_CS) )
  {
    while(1); // Leave the STAT LED on to indicate an error
  }

  // Reset the device to default settings
  // This if helpful is you're doing multiple uploads testing different settings. 
  myISM.deviceReset();

  // Wait for it to finish reseting
  while( !myISM.getDeviceReset() ){ 
    delay(1);
  } 

  delay(100);

  myISM.setAccelDataRate(ISM_XL_ODR_OFF); // Disable the accelerometers
  myISM.setAccelDataRate(ISM_GY_ODR_OFF); // Disable the gyros

  delay(100);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Step 4: myMag.disableContinuousMode();

  SPISettings mySettings = SPISettings(3000000, MSBFIRST, SPI_MODE0);
  if ( !myMag.begin(MAG_CS, mySettings, SPI) ) // .begin sets up the CS pin as an OUTPUT
  {
    while (1); // Leave the STAT LED on to indicate an error
  }

  myMag.softReset();

  delay(100);

  myMag.disableContinuousMode();
  
  delay(100);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Step 2: Disable Wake-Up Sources (ALL) and Configure Sleep PowerDown (set _all_ domains to OFF)

  esp_sleep_config_gpio_isolate();

  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_OFF);

  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

  // -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

  // Step 1: Deep Sleep

  //digitalWrite(STAT_LED, LOW); // Turn the STAT LED off. Redundant since the pin is isolated during sleep.

  esp_deep_sleep_start();
}

void loop()
{
  // Nothing to do here
}
