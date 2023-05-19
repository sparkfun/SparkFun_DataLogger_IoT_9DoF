// SparkFun DataLogger IoT – 9DoF Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

/**********************************************************************************************
 *
 * WARNING!
 * 
 * This is a sketch we wrote to test the DataLogger IoT – 9DoF hardware.
 * Please think very carefully before uploading it to your DataLogger.
 * 
 * You will overwrite the DataLogger firmware, leaving it unable to update or restore itself. 
 * 
 * The DataLogger IoT – 9DoF comes pre-programmed with amazing firmware which can do _so_ much.
 * It is designed to be able to update itself and restore itself if necessary.
 * But it can not do that if you overwrite the firmware with this test sketch.
 * It is just like erasing the restore partition on your computer hard drive.
 * Do not do it - unless you really know what you are doing.
 * 
 * Really. We mean it.
 * 
 * Your friends at SparkFun.
 * 
 * License: MIT. Please see LICENSE.MD for more details
 * 
 **********************************************************************************************/

#define STAT_LED 25 // OpenLog ESP32 STAT LED is connected to D25

void setup()
{
  pinMode(STAT_LED, OUTPUT);
}

void loop()
{
  digitalWrite(STAT_LED, !digitalRead(STAT_LED)); // Toggle the STAT LED
  delay(500); // At 1Hz
}
