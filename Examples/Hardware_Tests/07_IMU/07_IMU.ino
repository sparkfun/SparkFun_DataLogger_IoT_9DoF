// OpenLog ESP32 Test Example
// Tested with Espressif ESP32 v2.0.5 and the "ESP32 Dev Module" board definition

//#define IMU_CS SS // The ISM330 chip select is connected to D5
uint8_t IMU_CS = 5;

#include <SPI.h>

#include "SparkFun_ISM330DHCX.h" //Click here to get the library: http://librarymanager/All#SparkFun_6DoF_ISM330DHCX

// SPI instance class call
SparkFun_ISM330DHCX_SPI myISM; 

// Structs for X,Y,Z data
sfe_ism_data_t accelData; 
sfe_ism_data_t gyroData; 

void setup(){

	SPI.begin();

	Serial.begin(115200);
	pinMode(IMU_CS, OUTPUT);
	digitalWrite(IMU_CS, HIGH);

  Serial.println(F("SparkFun OpenLog ESP32 Example"));

	if( !myISM.begin(IMU_CS) ){
		Serial.println(F("IMU did not begin. Freezing..."));
	  while(1);
	}

	// Reset the device to default settings
	// This if helpful is you're doing multiple uploads testing different settings. 
	myISM.deviceReset();

	// Wait for it to finish reseting
	while( !myISM.getDeviceReset() ){ 
		delay(1);
	} 

	Serial.println(F("IMU has been reset."));
	Serial.println(F("Applying settings..."));
	delay(100);
	
	myISM.setDeviceConfig();
	myISM.setBlockDataUpdate();
	
	// Set the output data rate and precision of the accelerometer
	myISM.setAccelDataRate(ISM_XL_ODR_104Hz);
	myISM.setAccelFullScale(ISM_4g); 

	// Set the output data rate and precision of the gyroscope
	myISM.setGyroDataRate(ISM_GY_ODR_104Hz);
	myISM.setGyroFullScale(ISM_500dps); 

	// Turn on the accelerometer's filter and apply settings. 
	myISM.setAccelFilterLP2();
	myISM.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);

	// Turn on the gyroscope's filter and apply settings. 
	myISM.setGyroFilterLP1();
	myISM.setGyroLP1Bandwidth(ISM_MEDIUM);
}

void loop(){

	// Check if both gyroscope and accelerometer data is available.
	if( myISM.checkStatus() ){
		myISM.getAccel(&accelData);
		myISM.getGyro(&gyroData);
		Serial.print(F("Accelerometer: "));
		Serial.print(F("X: "));
		Serial.print(accelData.xData);
		Serial.print(F(" "));
		Serial.print(F("Y: "));
		Serial.print(accelData.yData);
		Serial.print(F(" "));
		Serial.print(F("Z: "));
		Serial.print(accelData.zData);
		Serial.println(F(" "));
		Serial.print(F("Gyroscope: "));
		Serial.print("X: ");
		Serial.print(gyroData.xData);
		Serial.print(F(" "));
		Serial.print(F("Y: "));
		Serial.print(gyroData.yData);
		Serial.print(F(" "));
		Serial.print(F("Z: "));
		Serial.print(gyroData.zData);
		Serial.println(F(" "));
	}

	delay(100);
}
