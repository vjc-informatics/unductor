#include <NAxisMotion.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);

float roll;
float pitch;
float yaw;
float heading;
float initHeading;

NAxisMotion mySensor;         //Object that for the sensor
unsigned long lastStreamTime = 0;     //To store the last streamed time stamp
const int streamPeriod = 20;          //To stream at 50Hz without using additional timers (time period(ms) =1000/frequency(Hz))


void setup() {
  // LCD initialization
	lcd.init();
	lcd.backlight();
	lcd.print("LCD initialized");

  // Peripheral initialization for sensors
  Serial.begin(115200);
  I2C.begin();
  mySensor.initSensor();          //The I2C Address can be changed here inside this function in the library
  mySensor.setOperationMode(OPERATION_MODE_NDOF);   //Can be configured to other operation modes as desired
  mySensor.setUpdateMode(MANUAL);	//The default is AUTO. Changing to MANUAL requires calling the relevant update functions prior to calling the read functions
  //Setting to MANUAL requires fewer reads to the sensor
  Serial.println("For the next ten seconds, please rotate the ROV");
  while (millis() < 10000) {
    mySensor.updateEuler();        //Update the Euler data into the structure of the object
    mySensor.updateCalibStatus();  //Update the Calibration Status
    initHeading = mySensor.readEulerHeading();
  }

  Serial.println("Please place ROV on a flat surface facing away from you");
  Serial.println("Streaming in ...");	//Countdown
  Serial.print("3...");
  delay(1000);	//Wait for a second
  Serial.print("2...");
  delay(1000);	//Wait for a second
  Serial.println("1...");
  delay(1000);	//Wait for a second
	Serial.print("Initial heading is ");
	Serial.print(initHeading);
  Serial.println();

}

void loop() {
  if ((millis() - lastStreamTime) >= streamPeriod)
  {
    lastStreamTime = millis();
    mySensor.updateEuler();        //Update the Euler data into the structure of the object
    mySensor.updateMag();
    mySensor.updateCalibStatus();  //Update the Calibration Status

    Serial.print("Time: ");
    Serial.print(lastStreamTime);
    Serial.print("ms ");

    // EULER HEADINGS
    Serial.print(" Heading: ");
    heading = mySensor.readEulerHeading();
    Serial.print(mySensor.readEulerHeading()); //Heading data
    Serial.print("deg ");

    Serial.print(" Yaw: ");
		yaw = initHeading - heading;
		Serial.print(yaw);
		Serial.print("deg");

    Serial.print(" Roll: ");
    Serial.print(mySensor.readEulerRoll()); //Roll data
    Serial.print("deg");

    Serial.print(" Pitch: ");
    Serial.print(mySensor.readEulerPitch()); //Pitch data
    Serial.print("deg ");

    // COMPASS DATA AND CALCULATIONS

    Serial.print(" MagX: ");
    float MagX = mySensor.readMagX();
    Serial.print(MagX); //Pitch data
    Serial.print("T ");

    Serial.print(" MagY: ");
    float MagY = mySensor.readMagY();
    Serial.print(MagY); //Pitch data
    Serial.print("T ");

    Serial.print(" Compass: ");
    Serial.print(getHeading()); //Pitch data
    Serial.print("deg ");

    // CALIBRATION STATUS

    Serial.print(" A: ");
    Serial.print(mySensor.readAccelCalibStatus());  //Accelerometer Calibration Status (0 - 3)

    Serial.print(" M: ");
    Serial.print(mySensor.readMagCalibStatus());    //Magnetometer Calibration Status (0 - 3)

    Serial.print(" G: ");
    Serial.print(mySensor.readGyroCalibStatus());   //Gyroscope Calibration Status (0 - 3)

    Serial.print(" S: ");
    Serial.print(mySensor.readSystemCalibStatus());   //System Calibration Status (0 - 3)

    Serial.println();

    // Controlling the LCD display

    lcd.setCursor(0,0);
    lcd.print("Heading: ");
    int head = round(heading);
    lcd.print(head);
    lcd.print(" deg  ");

    lcd.setCursor(0,1);
    lcd.print("Compass: ");
    int compass = round(getHeading());
    lcd.print(compass);
    lcd.print(" deg  ");

  }
}

float getHeading() {

  float MagX = mySensor.readMagX();
  float MagY = mySensor.readMagY();

  // Grabs magnetometer data and spits out a heading
  float heading = atan2(MagX, MagY);
  if(heading < 0) heading += 2*PI;
  if(heading > 2*PI) heading -= 2*PI;

  return heading * RAD_TO_DEG; //radians to degrees

}
