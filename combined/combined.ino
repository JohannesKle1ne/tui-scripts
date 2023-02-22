#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"
#include "Wire.h"  // This library allows you to communicate with I2C devices.

TMRpcm tmrpcm;


const int MPU_ADDR = 0x68;  // I2C address of the MPU-6050. If AD0 pin is set to HIGH, the I2C address will be 0x69.

int16_t accelerometer_x, accelerometer_y, accelerometer_z;  // variables for accelerometer raw data
int16_t gyro_x, gyro_y, gyro_z;                             // variables for gyro raw data
//int16_t temperature; // variables for temperature data

char tmp_str[7];  // temporary variable used in convert function

int currentPlayingSide = 0;
bool running = true;

int speaker = 7;

int tolerance = 5000;

int side1[3] = { 0, 0, 15000 };
int side2[3] = { 0, 0, -15000 };
int side3[3] = { 0, 15000, 0 };
int side4[3] = { 0, -15000, 0 };

bool checkSide(int side[3], int x, int y, int z) {
  if (abs(side[0] - x) < tolerance && abs(side[1] - y) < tolerance && abs(side[2] - z) < tolerance) {
    return true;
  }
  return false;
}

int getCurrentSide(int x, int y, int z) {
  if (checkSide(side1, x, y, z)){
    return 1;
  }
  if (checkSide(side2, x, y, z)){
    return 2;
  }
  if (checkSide(side3, x, y, z)){
    return 3;
  }
  if (checkSide(side4, x, y, z)){
    return 4;
  }
  return 0;
}

char* convert_int16_to_str(int16_t i) {  // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

// char* getSong(char* type) {
//   if ("active" == type) {
//     return "Active_2.wav";
//   }
//   if ("relaxed" == type) {
//     return "Relaxed_2.wav";
//   }
//   if ("concentrated" == type) {
//     return "Concentrated_1.wav";
//   }
//   if ("productive" == type) {
//     return "Productive_1.wav";
//   }
//   if ("social" == type) {
//     return "Social_1.wav";
//   }
//   if ("creative" == type) {
//     return "Creative_1.wav";
//   }
//   if ("mindful" == type) {
//     return "Mindful_1.wav";
//   }
//   if ("amusing" == type) {
//     return "Amusing_1.wav";
//   }
// }

// void handleSide(int number, int side[3], char* mood) {
//   Serial.println("Found: " + number);
//   delay(1000);
//   bool newResult = check(side, accelerometer_x, accelerometer_y, accelerometer_z);
//   if (newResult && currentPlayingSide != number) {
//     Serial.println("Play Music for Side " + number);
//     Serial.println(getSong(mood));

//     tmrpcm.play(getSong(mood));
//     //tmrpcm.play("Active_2.wav");
//     currentPlayingSide = number;
//   }
// }


void setup() {

  tmrpcm.speakerPin = 9;
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }
  tmrpcm.setVolume(1);



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
    
    int sideNumber = getCurrentSide(accelerometer_x, accelerometer_y, accelerometer_z);

    if (sideNumber==1) {
      if (currentPlayingSide != sideNumber) {
        Serial.print("Play Music for Side: ");
        Serial.println("1");
        Serial.println("Active_2.wav");
        tmrpcm.play("Active_2.wav");
        currentPlayingSide = sideNumber;
      }
    }
    if (sideNumber==2) {
      if (currentPlayingSide != sideNumber) {
        Serial.print("Play Music for Side: ");
        Serial.println("2");
        Serial.println("Productive_1.wav");
        tmrpcm.play("Productive_1.wav");
        currentPlayingSide = sideNumber;
      }
    }

    //Serial.println("Running");
    //bool resultTesting = check(sideTesting, accelerometer_x, accelerometer_y, accelerometer_z);

    // if (resultTesting) {
    //   Serial.println("Found Test");
    //   delay(1000);
    //   bool newResult = check(sideTesting, accelerometer_x, accelerometer_y, accelerometer_z);
    //   if (newResult && currentPlayingSide != 9) {
    //     Serial.println("Play Music for Side 9 (Test)");
    //     tmrpcm.play("test.wav");
    //     //tone(speaker, 1000);
    //     currentPlayingSide = 9;
    //   }
    // }
    // if (result7) {
    //   handleSide(7, side7, "creative");
    // }
    // if (result7) {
    //   Serial.println("Found 7");
    //   delay(1000);
    //   bool newResult = check(side7, accelerometer_x, accelerometer_y, accelerometer_z);
    //   if (newResult && currentPlayingSide != 7) {
    //     Serial.println("Play Music for Side 7");
    //     tmrpcm.play(getSong("relaxed"));
    //     currentPlayingSide = 7;
    //   }
    // }
    // if (result6) {
    //   Serial.println("Found 6");
    //   delay(1000);
    //   bool newResult = check(side6, accelerometer_x, accelerometer_y, accelerometer_z);
    //   if (newResult && currentPlayingSide != 6) {
    //     Serial.println("Play Music for Side 7");
    //     tmrpcm.play(getSong("relaxed"));
    //     currentPlayingSide = 7;
    //   }
    // }
    if (sideNumber==0) {
      Serial.println("Stop playing");
      tmrpcm.disable();
      noTone(speaker);
      currentPlayingSide = 0;
    }
  } else {
    noTone(speaker);
    currentPlayingSide = 0;
  }


  if (abs(gyro_y) > 10000 || abs(gyro_z) > 10000) {
    //Serial.println("Stop/Start Music");
    //running = !running;
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