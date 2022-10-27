// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

#define EN_3V3_SW 32 // The 3.3V_SW regulator Enable pin is connected to D32

void setup() {
  pinMode(EN_3V3_SW, OUTPUT);
  digitalWrite(EN_3V3_SW, HIGH);
}

void loop()
{
  // Nothing to do here...
}
