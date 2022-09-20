#include <QuickMedianLib.h>
#include <Servo.h>

#define p1  0.0001066
#define p2  -0.1252
#define p3  44.02


#define distPin A3

#define xServoPin 9
#define yServoPin 10

#define prereadDelay 40
#define readDelay 20
#define readSamples 5

#define baud 9600
#define serialDelay 20

#define maxMessageSize 32

#define moveDelay 500

Servo xServo, yServo;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(baud);
  xServo.attach(xServoPin);
  yServo.attach(yServoPin);
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
  float fraw = float(raw);
  float out = p1*fraw*fraw+p2*fraw+p3;
  return(round(out));
}


int convertPos2Raw(int pos){
  return(map(pos,0,90,500,1500));
}

void setPos(int _pan, int _tilt){
  xServo.writeMicroseconds(convertPos2Raw(_pan));
  yServo.writeMicroseconds(convertPos2Raw(_tilt));
  delay(moveDelay);
}

void handleRead(int _pan, int _tilt){
  setPos(_pan,_tilt);
  Serial.print(_pan);
  Serial.print(",");
  Serial.print(_tilt);
  Serial.print(",");
  Serial.println(readDistNow());
  
}

char message[maxMessageSize];
int pan = 0;
int tilt = 0;
bool finished = false;
void loop() {
  if(Serial.available() >0 ){
    Serial.println("ser avail");
    String currentStr = "";
    delay(serialDelay);
    char currentLetter;
    do{
      currentLetter = Serial.read();
      delay(1);
      //Serial.println(currentLetter);
      if(currentLetter==','){
        pan = currentStr.toInt();
        currentStr="";
      }else if(currentLetter==13||Serial.available() <=0){
        tilt = currentStr.toInt();
        currentStr="";
        handleRead(pan,tilt);
        finished=true;
      }else{
        currentStr+=currentLetter;
      }
      //Serial.print(currentStr);
    }while(!finished);
    finished=false;
    delay(500);
  }
  //setPos(pan,tilt);
  //Serial.println(readDistNow());
  delay(500);
  //Serial.println("waiting");
  // put your main code here, to run repeatedly:

}
