#include <QuickMedianLib.h>

#define distPin A3

#define xServo 9
#define yServo 10

#define prereadDelay 40
#define readDelay 20
#define readSamples 5

#define baud 9600

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baud);
  delay(1000);
  Serial.println(F("pie mp2 scanner"));
}

int readDistNow(){
  int dataPoints[readSamples];
  delay(prereadDelay);
  for(uint8_t i=0;i<readSamples;i++){
    dataPoints[i] = analogRead(distPin);
    delay(readDelay);
  }
  int rawDist = QuickMedian<int>::GetMedian(dataPoints, readSamples);
  int dist = convertFromRaw(rawDist);
  return(dist);
  
}

int convertFromRaw(int raw){
  return(raw); // TODO just a pass through rn
}



void loop() {
  Serial.println(readDistNow());
  delay(1000);
  // put your main code here, to run repeatedly:

}
