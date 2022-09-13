#include <QuickMedianLib.h>

#define distPin A3

#define xServo 9
#define yServo 10

#define prereadDelay 40
#define readDelay 20
#define readSamples 5
#define outliersRemoved 1



void setup() {
  // put your setup code here, to run once:

}

int readDistNow(){
  int dataPoints[readSamples];
  int maxPoints[outliersRemoved];
  int minPoints[outliersRemoved];
  delay(prereadDelay 40);
  for(uint8_t i=0;i<readsamples;i++){
    dataPoints[i] = analogRead(distPin);
    delay(readDelay);
  }
  
}



void loop() {
  // put your main code here, to run repeatedly:

}
