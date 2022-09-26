#include <QuickMedianLib.h>
#include <Servo.h>

#define p1  0.0001066 //from fit poly2
#define p2  -0.1252
#define p3  44.02


#define distPin A3

#define xServoPin 9 //pan/phi
#define yServoPin 10 // tilt/theta

#define prereadDelay 0// ms before taking samples
#define readDelay 1 // ms between samples
#define readSamples 5// numb of samples to tae median of

#define baud 115200
#define serialDelay 1

#define maxMessageSize 32

#define moveSpeed 5 // ms/deg (from servo datasheet)
#define minMoveDelay 0 // min delay of a move (added on to speed based delay)
Servo xServo, yServo;

void setup() {
  Serial.begin(baud);
  xServo.attach(xServoPin);
  yServo.attach(yServoPin);
  delay(10);
}


int readDistNow(){
  int dataPoints[readSamples];// init array of datapoints
  delay(prereadDelay); // delay for servo overshoot or other issues
  for(uint8_t i=0;i<readSamples;i++){ // loop to collect samples
    dataPoints[i] = analogRead(distPin);
    delay(readDelay); // wait before next sample 
  }
  int rawDist = QuickMedian<int>::GetMedian(dataPoints, readSamples); // get the median so the outlier is thrown out
  int dist = convertFromRaw(rawDist); // convert to 16ths of an inch
  return(dist);
  
}

int prev_pan = 90; // for calculating move times
int prev_tilt = 90;

int convertFromRaw(int raw){//16ths
  float fraw = float(raw);
  float out = (p1*fraw*fraw+p2*fraw+p3)*16; //  fit equation from matlab
  return(round(out)); // could have been float but i didnt want to change it for fear of breaking it
}


int convertPos2Raw(int pos){
  return(map(pos,0,90,500,1500)); // ik that servo lib does this but i have more control on limits this way
}

void setPos(int _pan, int _tilt){
  xServo.writeMicroseconds(convertPos2Raw(_pan));
  yServo.writeMicroseconds(convertPos2Raw(_tilt));
  delay((max(abs(prev_pan-_pan),abs(prev_tilt-_tilt))*moveSpeed)+minMoveDelay); // find the max time for both servos and wait that time
  prev_pan = _pan;
  prev_tilt = _tilt;// reset values
}

void handleRead(int _pan, int _tilt){// print the 2 cords and the values
  setPos(_pan,_tilt);// set pos (and wait until there)
  Serial.print(_pan);
  Serial.print(",");
  Serial.print(_tilt); // could have done this in less lines
  Serial.print(",");
  Serial.println(readDistNow()); 
  
}

char message[maxMessageSize];
int pan = 0;
int tilt = 0;
bool finished = false;
void loop() {  // i could have refactored this to be faster but i didnt have the time to
  if(Serial.available() >0 ){ // wait for serial input
    String currentStr = "";
    delay(serialDelay); // wait for most of the message to come in
    char currentLetter;
    do{
      currentLetter = Serial.read(); // set char
      delay(1);
      if(currentLetter==','){ // use the comma to delimit the str
        pan = currentStr.toInt(); // save current as the pan
        currentStr=""; //            and reset
      }else if(currentLetter==13||Serial.available() <1){
        tilt = currentStr.toInt();// save current as the pan
        currentStr=""; //            and reset
        handleRead(pan,tilt); // then handle the read and send it to matlab
        finished=true;       // flag to get out of loop
      }else{
        currentStr+=currentLetter; // otherwise keep moving on
      }
    }while(!finished); // look for flag to get out of loop
    finished=false;
    delay(2);// wait for a while after message finished
  }
  delay(2);// wait until next message

}
