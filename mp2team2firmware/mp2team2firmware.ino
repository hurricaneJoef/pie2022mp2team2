#include <QuickMedianLib.h>

#define distPin A3

#define xServoPin 9
#define yServoPin 10

#define prereadDelay 40
#define readDelay 20
#define readSamples 5

#define baud 9600
#define serialDelay 20

#define maxMessagesize 32

#define moveDelay 500

Servo xServo, yServo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baud);
  xServo.attach(xServoPin);
  ySefvo.attach(yServoPin);
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


void sendData(int pan, int tilt, int dist){
  
}

int convertPos2Raw(int pos){
  return pos;
}

void setPos(int _pan, int _tilt){
  xServo.writeMicroseconds(convertPos2Raw(_pan));
  yServo.writeMicroseconds(convertPos2Raw(_tilt));
  delay(moveDelay);
}

void handleRead(int _pan, int _tilt){
  setPos(pan,tilt);
  Serial.print(pan);
  Serial.print(",");
  Serial.print(tilt);
  Serial.println(readDistNow());
  
}

char message[maxMessageSize);
int prevMessageSize = 0;
int pan = 0;
int tilt = 0;
void loop() {
  if(Serial.available() >0 ){
    String currentStr = "";
    delay(serialDelay);
    char currentLetter = Serial.read();
    do{
      if(currentletter==','){
        pan = currentStr.toInt();
        currentStr="";
      }else if(currentletter==13){
        tilt = currentStr.toInt();
        currentStr="";
        
      }else{
        currentStr+=currentletter;
      }
    }while(!finished)
    if(prevMessageSize>=maxMessageSize){
      Serial.println(F("ERROR TOO MANY CHAR"));
    }
  }
  setPos(pan,tilt);
  Serial.println(readDistNow());
  delay(1000);
  // put your main code here, to run repeatedly:

}
