#include <QuickMedianLib.h>
#include <Servo.h>

#define p1  0.0001066 //from fit poly2
#define p2  -0.1252
#define p3  44.02


#define distPin A3

#define xServoPin 9 //pan/phi
#define yServoPin 10 // tilt/theta

#define readDelay 1 // ms between samples
#define readSamples 5// numb of samples to tae median of

#define baud 115200

#define maxMessageSize 32

#define moveSpeed 5 // ms/deg (from servo datasheet)
#define xtraMoveDelay 0 // extra delay of a move (added on to speed based delay)
Servo xServo, yServo;



float readDistNow(){
  int dataPoints[readSamples];// init array of datapoints
  for(uint8_t i=0;i<readSamples;i++){ // loop to collect samples
    dataPoints[i] = analogRead(distPin);
    delay(readDelay); // wait before next sample 
  }
  int rawDist = QuickMedian<int>::GetMedian(dataPoints, readSamples); // get the median so the outlier is thrown out
  float dist = convertFromRaw(rawDist); // convert to 16ths of an inch
  return(dist);
  
}

int prev_pan = 0; // for calculating move times
int prev_tilt = 0;

int convertFromRaw(int raw){//16ths
  float fraw = float(raw);
  float out = (p1*fraw*fraw+p2*fraw+p3)*16; //  fit equation from matlab
  return(out); // could have been float but i didnt want to change it for fear of breaking it
}


int convertPos2Raw(int pos){
  return(map(pos,0,90,500,1500)); // ik that servo lib does this but i have more control on limits this way
}

void setPos(int _pan, int _tilt){
  xServo.writeMicroseconds(convertPos2Raw(_pan));
  yServo.writeMicroseconds(convertPos2Raw(_tilt));
  delay((max(abs(prev_pan-_pan),abs(prev_tilt-_tilt))*moveSpeed)+xtraMoveDelay); // find the max time for both servos and wait that time
  prev_pan = _pan;
  prev_tilt = _tilt;// reset values
}

void handleRead(int _pan, int _tilt){// print the 2 cords and the values
  setPos(_pan,_tilt);// set pos (and wait until there)
  Serial.println(String(_pan)+F(",")+String(_tilt)+F(",")+String(readDistNow()));
  //Serial.print(_pan);
  //Serial.print(",");
  //Serial.print(_tilt); 
  //Serial.print(",");
  //Serial.println(readDistNow()); 
  
}


char message[maxMessageSize];
int pan = 0;
int tilt = 0;
char currentLetter;
String currentStr = "";


void setup() {
  Serial.begin(baud);
  xServo.attach(xServoPin);
  yServo.attach(yServoPin);
  delay(10);
  setPos(pan,tilt);
}


void loop() {
  if(Serial.available() >0 ){ // wait for serial input
      currentLetter = Serial.read(); // set char
      if(currentLetter==','){ // use the comma to delimit the str
        pan = currentStr.toInt(); // save current as the pan
        currentStr=""; //            and reset
      }else if(currentLetter==13 && pan>=-999){ // see if flag for no pan number yet if so ignore
        tilt = currentStr.toInt();// save current as the pan
        currentStr=""; //            and reset
        handleRead(pan,tilt); // then handle the read and send it to matlab
        pan=-1000; // could have been -1 but then you couldnt push the limits of the system so crazy low num instead
      }else{
        currentStr+=currentLetter; // otherwise keep moving on
      }
  }
// do nothing while no serial
}
