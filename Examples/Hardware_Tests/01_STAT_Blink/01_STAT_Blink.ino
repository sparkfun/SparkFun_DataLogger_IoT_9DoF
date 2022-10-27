// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

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
