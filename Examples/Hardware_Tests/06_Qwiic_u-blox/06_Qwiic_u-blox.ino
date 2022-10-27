// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

#define EN_3V3_SW 32 // The 3.3V_SW regulator Enable pin is connected to D32

#include <Wire.h> //Needed for I2C to GPS

#include "SparkFun_u-blox_GNSS_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GNSS myGNSS;

void setup()
{
  pinMode(EN_3V3_SW, OUTPUT); // Enable Qwiic power and I2C
  digitalWrite(EN_3V3_SW, HIGH);

  delay(1000); // Allow time for the u-blox module to power up

  Serial.begin(115200);
  Serial.println("SparkFun u-blox Example");

  Wire.begin();

  if (myGNSS.begin() == false)
  {
    Serial.println(F("u-blox GNSS module not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }

  //This will pipe all NMEA sentences to the serial port so we can see them
  myGNSS.setNMEAOutputPort(Serial);
}

void loop()
{
  myGNSS.checkUblox(); //See if new data is available. Process bytes as they come in.

  delay(250); //Don't pound too hard on the I2C bus
}
