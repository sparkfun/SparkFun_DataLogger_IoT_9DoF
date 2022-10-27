// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition


// Set the IMU accel and gyro to run at 1666Hz.
// Configure the IMU so it generates an interrupt on accel Data Ready.
// Each time an IMU interrupt is detected, read the Accel and Gyro data.
// When writing the data to SD card, don't do anything too 'fancy' (like using a separate
// thread for the writer). Just write the data in blocks of 512 bytes.... And see what happens!
  

#define EN_3V3_SW 32 // The 3.3V_SW regulator Enable pin is connected to D32

#include "FS.h"
#include "SD_MMC.h"
File file;



//#define IMU_CS SS // The ISM330 chip select is connected to D5
int IMU_CS = 5;
int IMU_INT1 = 34; // The IMU INT1 is connected to D34

#include <SPI.h>

#include <SparkFun_ISM330DHCX.h> //Click here to get the library: http://librarymanager/All#SparkFun_6DoF_ISM330DHCX

// SPI instance class call
SparkFun_ISM330DHCX_SPI myISM; 

// Structs for X,Y,Z data
sfe_ism_data_t accelData; 
sfe_ism_data_t gyroData;

static volatile bool IMUInt1Seen = false; // Flag to indicate if an IMU interrupt has been seen

void IMUInt1ISR() // Interrupt Service Routine
{
  IMUInt1Seen = true;
}



//#define MAG_CS 27 // The MMC5983 chip select is connected to D27
int MAG_CS = 27;



void setup()
{
  pinMode(EN_3V3_SW, OUTPUT); // Enable power for the microSD card
  digitalWrite(EN_3V3_SW, HIGH);
  pinMode(IMU_CS, OUTPUT);
  digitalWrite(IMU_CS, HIGH);
  pinMode(MAG_CS, OUTPUT);
  digitalWrite(MAG_CS, HIGH); // De-select the mag
  pinMode(IMU_INT1, INPUT);

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
  if (SD_MMC.remove("/IMU.csv"))
      Serial.println("File deleted.");

  // Open the log file for writing
  file = SD_MMC.open("/IMU.csv", FILE_WRITE);
  if(!file)
  {
    Serial.println(F("Failed to open log file for writing. Freezing..."));
    while(1);
  }


  // Begin the IMU
  if( !myISM.begin(IMU_CS) )
  {
    Serial.println(F("IMU did not begin. Freezing..."));
    while(1);
  }

  // Reset the device to default settings
  // This if helpful is you're doing multiple uploads testing different settings. 
  myISM.deviceReset();

  // Wait for it to finish reseting
  while( !myISM.getDeviceReset() )
  { 
    delay(1);
  } 

  Serial.println(F("IMU has been reset."));
  Serial.println(F("Applying settings..."));
  delay(100);
  
  myISM.setDeviceConfig();
  myISM.setBlockDataUpdate();

  // Set the output data rate and precision of the accelerometer
  //myISM.setAccelDataRate(ISM_XL_ODR_6667Hz);
  myISM.setAccelDataRate(ISM_XL_ODR_1666Hz);
  myISM.setAccelFullScale(ISM_4g); 

  // Turn on the accelerometer's filter and apply settings. 
  //myISM.setAccelFilterLP2();
  //myISM.setAccelSlopeFilter(ISM_LP_ODR_DIV_10);

  // Set the output data rate and precision of the gyroscope
  //myISM.setGyroDataRate(ISM_GY_ODR_6667Hz);
  myISM.setGyroDataRate(ISM_GY_ODR_1666Hz);
  myISM.setGyroFullScale(ISM_500dps);

  // Set the accelerometer's status i.e. the data ready to interrupt one. 
  myISM.setAccelStatustoInt1();

  // This function call will modify which "events" trigger an interrupt.
  myISM.setIntNotification(ISM_ALL_INT_PULSED);

  myISM.setDataReadyMode(1); // Set data ready mode to pulsed

  myISM.setPinMode(false); // Set the pin mode to push-pull, active high

  // Attach the interrupt
  attachInterrupt(digitalPinToInterrupt(IMU_INT1), IMUInt1ISR, RISING);

  
  Serial.println(F("Here we go! Logging for 10 seconds..."));
}

void loop()
{
  unsigned long startTime = millis(); // Record the start time
  unsigned long counter = 0; // Record the number of samples
  unsigned long lastMicros = micros(); // Record the sample/write interval

  char csvLine[2 * 512] = {0}; // Storage for the CSV data
  char *csvLinePtr = csvLine; // Pointer to the end of the data

  // Each time an IMU interrupt is detected, read the Accel and Gyro data.
  // Write the text data in blocks of 512 bytes.
  
  while (millis() < (startTime + 10000)) // Record data for 10 seconds
  {
    if (IMUInt1Seen) // Have we seen an interrupt?
    {
      IMUInt1Seen = false; // Clear the flag
      
      myISM.getAccel(&accelData);
      myISM.getGyro(&gyroData);

      unsigned long microsNow = micros();
      
      csvLinePtr += sprintf(csvLinePtr, "%lu,%lu,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r\n",
        microsNow, microsNow - lastMicros,
        accelData.xData, accelData.yData, accelData.zData,
        gyroData.xData, gyroData.yData, gyroData.zData);

      if (csvLinePtr >= (csvLine + 512)) // Have we got enough data to write to SD card?
      {
        file.write((const uint8_t *)csvLine, 512); // Write a block of 512 bytes
        memcpy(csvLine, (const char *)(csvLine + 512), (csvLinePtr - (csvLine + 512)) + 1); // Move the remaining text - plus the NULL - to the start of csvLine
        csvLinePtr -= 512; // Update the pointer
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
