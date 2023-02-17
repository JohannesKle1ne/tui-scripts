#include "Wire.h"  // This library allows you to communicate with I2C devices.

const int MPU_ADDR = 0x68;  // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z;  // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z;                             // variables for gyro raw data
//int16_t temperature; // variables for temperature data

char tmp_str[7];  // temporary variable used in convert function

char* convert_int16_to_str(int16_t i) {  // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

bool check(int side[3], int x, int y, int z) {

  int tolerance = 500;

  if (abs(side[0] - x) < tolerance && abs(side[1] - y) < tolerance && abs(side[2] - z) < tolerance) {
    return true;
  }
  return false;
}

int sideTesting[3] = { 0, 0, 15500 };
int side8[3] = { -14000, 2000, -8000 };
int side7[3] = { -5500, 15000, 2500 };

int currentPlayingSide = 0;
bool running = false;

int speaker = 7;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);  // Begins a transmission to the I2C slave (GY-521 board)
  Wire.write(0x6B);                  // PWR_MGMT_1 register
  Wire.write(0);                     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  pinMode(speaker, OUTPUT);
}


void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                         // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false);              // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU_ADDR, 7 * 2, true);  // request a total of 7*2=14 registers

  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  accelerometer_x = Wire.read() << 8 | Wire.read();  // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  accelerometer_y = Wire.read() << 8 | Wire.read();  // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  accelerometer_z = Wire.read() << 8 | Wire.read();  // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  //temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  gyro_x = Wire.read() << 8 | Wire.read();  // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  gyro_y = Wire.read() << 8 | Wire.read();  // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  gyro_z = Wire.read() << 8 | Wire.read();  // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)


  if (running) {
    Serial.println("Running");
    bool resultTesting = check(sideTesting, accelerometer_x, accelerometer_y, accelerometer_z);
    bool result8 = check(side8, accelerometer_x, accelerometer_y, accelerometer_z);
    bool result7 = check(side7, accelerometer_x, accelerometer_y, accelerometer_z);

    if (resultTesting) {
      Serial.println("Found Test");
      delay(3000);
      bool newResult = check(sideTesting, accelerometer_x, accelerometer_y, accelerometer_z);
      if (newResult && currentPlayingSide != 9) {
        Serial.println("Play Music for Side 9 (Test)");
        tone(speaker, 1000);
        currentPlayingSide = 9;
      }
    }
    if (result8) {
      Serial.println("Found 8");
      delay(3000);
      bool newResult = check(side8, accelerometer_x, accelerometer_y, accelerometer_z);
      if (newResult && currentPlayingSide != 8) {
        Serial.println("Play Music for Side 8");
        currentPlayingSide = 8;
      }
    }
    if (result7) {
      Serial.println("Found 7");
      delay(3000);
      bool newResult = check(side7, accelerometer_x, accelerometer_y, accelerometer_z);
      if (newResult && currentPlayingSide != 7) {
        Serial.println("Play Music for Side 7");
        currentPlayingSide = 7;
      }
    }
    if(!resultTesting && !result8 && !result7){
      noTone(speaker);
      currentPlayingSide = 0;
    }
  }else{
    noTone(speaker);
    currentPlayingSide = 0;
  }


  if (abs(gyro_y) > 10000 || abs(gyro_z) > 10000) {
    Serial.println("Stop/Start Music");
    running = !running;
  }


  // print out data
  Serial.print("aX = ");
  Serial.print(convert_int16_to_str(accelerometer_x));
  Serial.print(" | aY = ");
  Serial.print(convert_int16_to_str(accelerometer_y));
  Serial.print(" | aZ = ");
  Serial.print(convert_int16_to_str(accelerometer_z));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  //Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = ");
  Serial.print(convert_int16_to_str(gyro_x));
  Serial.print(" | gY = ");
  Serial.print(convert_int16_to_str(gyro_y));
  Serial.print(" | gZ = ");
  Serial.print(convert_int16_to_str(gyro_z));
  Serial.println();

  // delay
  delay(1000);
}