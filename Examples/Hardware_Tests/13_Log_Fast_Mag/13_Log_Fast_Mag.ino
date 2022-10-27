// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.4 and the "ESP32 Dev Module" board definition


// Set the Mag to run at 1000Hz.
// Configure the Mag so it generates an interrupt on its INT pin.
// ** Solder the MAG INT - D35 jumper closed **
// Each time a Mag interrupt is detected, read the field data.
// When writing the data to SD card, don't do anything too 'fancy' (like using a separate
// thread for the writer). Just write the data in blocks of WRITE_BLOCK bytes.... And see what happens!
  

#define WRITE_BLOCK 512


#define EN_3V3_SW 32 // The 3.3V_SW regulator Enable pin is connected to D32

#include "FS.h"
#include "SD_MMC.h"
File file;


#include <SPI.h>

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA

SFE_MMC5983MA myMag;


volatile bool newDataAvailable = true;
uint32_t rawValueX = 0;
uint32_t rawValueY = 0;
uint32_t rawValueZ = 0;
double normalizedX = 0;
double normalizedY = 0;
double normalizedZ = 0;
double heading = 0;



//#define MAG_CS 27 // The MMC5983 chip select is connected to D27
int MAG_CS = 27;
int MAG_INT = 35; // The Mag INT is connected to D35  ** Solder the MAG INT - D35 jumper closed **



//#define IMU_CS SS // The ISM330 chip select is connected to D5
int IMU_CS = 5;
int IMU_INT1 = 34; // The IMU INT1 is connected to D34



void interruptRoutine()
{
    newDataAvailable = true;
}


void setup()
{
  pinMode(EN_3V3_SW, OUTPUT); // Enable power for the microSD card
  digitalWrite(EN_3V3_SW, HIGH);
  pinMode(IMU_CS, OUTPUT);
  digitalWrite(IMU_CS, HIGH);
  pinMode(MAG_CS, OUTPUT);
  digitalWrite(MAG_CS, HIGH); // De-select the mag
  pinMode(IMU_INT1, INPUT);
  pinMode(MAG_INT, INPUT);

  delay(1000); // Allow time for the SD card to start up

  SPI.begin();

  Serial.begin(115200);

  Serial.println(F("SparkFun OpenLog ESP32 Example"));

  // Begin the SD card
  if(!SD_MMC.begin())
  {
    Serial.println("Card mount failed. Freezing...");
    while(1);
  }

  // Delete an existing log file
  if (SD_MMC.remove("/MAG.csv"))
      Serial.println("File deleted.");

  // Open the log file for writing
  file = SD_MMC.open("/MAG.csv", FILE_WRITE);
  if(!file)
  {
    Serial.println(F("Failed to open log file for writing. Freezing..."));
    while(1);
  }


  // Begin the Mag
  if( !myMag.begin(MAG_CS) )
  {
    Serial.println(F("Mag did not begin. Freezing..."));
    while(1);
  }

  myMag.softReset();

  Serial.println("MMC5983MA connected");

  Serial.println("Setting filter bandwith to 800 Hz for continuous operation...");
  myMag.setFilterBandwidth(800);
  Serial.print("Reading back filter bandwith: ");
  Serial.println(myMag.getFilterBandwith());

  Serial.println("Setting continuous measurement frequency to 1000 Hz...");
  myMag.setContinuousModeFrequency(1000);
  Serial.print("Reading back continuous measurement frequency: ");
  Serial.println(myMag.getContinuousModeFrequency());

  Serial.println("Enabling auto set/reset...");
  myMag.enableAutomaticSetReset();
  Serial.print("Reading back automatic set/reset: ");
  Serial.println(myMag.isAutomaticSetResetEnabled() ? "enabled" : "disabled");

  Serial.println("Enabling continuous mode...");
  myMag.enableContinuousMode();
  Serial.print("Reading back continuous mode: ");
  Serial.println(myMag.isContinuousModeEnabled() ? "enabled" : "disabled");

  Serial.println("Enabling interrupt...");
  myMag.enableInterrupt();
  Serial.print("Reading back interrupt status: ");
  Serial.println(myMag.isInterruptEnabled() ? "enabled" : "disabled");

  // Attach the interrupt
  attachInterrupt(digitalPinToInterrupt(MAG_INT), interruptRoutine, RISING);

  // Set our interrupt flag, just in case we missed the rising edge
  newDataAvailable = true;

  
  Serial.println(F("Here we go! Logging for 10 seconds..."));
}

void loop()
{
  unsigned long startTime = millis(); // Record the start time
  unsigned long counter = 0; // Record the number of samples
  unsigned long lastMicros = micros(); // Record the sample/write interval

  char csvLine[2 * WRITE_BLOCK] = {0}; // Storage for the CSV data
  char *csvLinePtr = csvLine; // Pointer to the end of the data

  while (millis() < (startTime + 10000)) // Record data for 10 seconds
  {
    if (newDataAvailable == true) // Have we seen an interrupt?
    {
      newDataAvailable = false; // Clear our interrupt flag
      myMag.clearMeasDoneInterrupt(); // Clear the MMC5983 interrupt

      // Read all three channels simultaneously
      // Note: we are calling readFieldsXYZ to read the fields, not getMeasurementXYZ
      // The measurement is already complete, we do not need to start a new one
      myMag.readFieldsXYZ(&rawValueX, &rawValueY, &rawValueZ);
  
      // The magnetic field values are 18-bit unsigned. The zero (mid) point is 2^17 (131072).
      // Normalize each field to +/- 1.0
      normalizedX = (double)rawValueX - 131072.0;
      normalizedX /= 131072.0;
  
      normalizedY = (double)rawValueY - 131072.0;
      normalizedY /= 131072.0;
  
      normalizedZ = (double)rawValueZ - 131072.0;
      normalizedZ /= 131072.0;
    
      unsigned long microsNow = micros();
      
      csvLinePtr += sprintf(csvLinePtr, "%lu,%lu,%.1f,%.1f,%.1f\r\n",
        microsNow, microsNow - lastMicros,
        normalizedX, normalizedY, normalizedZ);

      if (csvLinePtr >= (csvLine + WRITE_BLOCK)) // Have we got enough data to write to SD card?
      {
        file.write((const uint8_t *)csvLine, WRITE_BLOCK); // Write a block of WRITE_BLOCK bytes
        memcpy(csvLine, (const char *)(csvLine + WRITE_BLOCK), (csvLinePtr - (csvLine + WRITE_BLOCK)) + 1); // Move the remaining text - plus the NULL - to the start of csvLine
        csvLinePtr -= WRITE_BLOCK; // Update the pointer
      }

      lastMicros = microsNow;
      counter++;
    }
  }

  file.write((const uint8_t *)csvLine, csvLinePtr - csvLine); // Write any remaining data
  file.close();

  Serial.print(F("Wrote "));
  Serial.print(counter);
  Serial.println(F(" lines to IMU.csv"));

  while(1); // Do nothing more
}
