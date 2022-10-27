// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

#define BOOT_BUTTON 0 // OpenLog ESP32 Boot Button is connected to D0
#define STAT_LED 25 // OpenLog ESP32 STAT LED is connected to D25

void setup()
{
  pinMode(BOOT_BUTTON, INPUT);
  pinMode(STAT_LED, OUTPUT);
}

void loop()
{
  // LED should be on by default
  // Pressing the button should turn the LED off
  digitalWrite(STAT_LED, digitalRead(BOOT_BUTTON));
}
