#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"

TMRpcm tmrpcm;

void setup()
{
tmrpcm.speakerPin=9;
Serial.begin(9600);
if(!SD.begin(SD_ChipSelectPin))
{
  Serial.println("SD fail");
  return;
}
tmrpcm.setVolume(1);
Serial.println("Play");
//tmrpcm.play("techno.wav"); //works

//tmrpcm.play("Productive_2.wav");
//tmrpcm.play("Concentrated_1.wav");
//tmrpcm.play("Amusing_2.wav");
//tmrpcm.play("Creative_1.wav");
//tmrpcm.play("Mindful_1.wav");
//tmrpcm.play("active1.wav"); //works
//tmrpcm.play("amusing1.wav"); //works
//tmrpcm.play("concentrated1.wav");
//tmrpcm.play("amusing2.wav"); //works
//tmrpcm.play("concentrated2.wav");

//tmrpcm.play("ac1.wav");
//tmrpcm.play("ac2.wav");
//tmrpcm.play("am1.wav");
//tmrpcm.play("am2.wav");
//tmrpcm.play("co1.wav");
//tmrpcm.play("co2.wav");
//tmrpcm.play("cr1.wav");
//tmrpcm.play("cr2.wav");
tmrpcm.play("mi1.wav");
//tmrpcm.play("mi2.wav");


















}

void loop() {
  // put your main code here, to run repeatedly:

}
