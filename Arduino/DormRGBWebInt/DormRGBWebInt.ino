/*
Corban Mailloux
 10/25/2012
 DormRGBWebInt.ino - RGB LED control over the web 
 (and with an Android app.)
 
 ****WARNING****             
 This program changes the PWM mode of Timer 0 in order to allow
 the LEDs PWM to work while a servo is attached.
 This means that the standard timing functions will not work.
 delay(), millis(), micros() will all have problems.
 
 See the setup() function for more information.
 
 */



/* THIS IS THE COLOR CROSS FADE CODE!!! */

// Color arrays
int black[3]  = { 
  0, 0, 0 };
int white[3]  = { 
  100, 100, 100 };
int red[3]    = { 
  100, 0, 0 };
int green[3]  = { 
  0, 100, 0 };
int blue[3]   = { 
  0, 0, 100 };
int yellow[3] = { 
  50, 50, 0 };
int dimWhite[3] = { 
  30, 30, 30 };
// etc.

// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];

int wait = 100;      // 10ms internal crossFade delay; increase for slower fades
int hold = 0;       // Optional hold when a color is complete, before the next crossFade

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;

/* END COLOR CROSSFADE CODE!!!  */




//PWM pins for the LEDs
#define redPin 5
#define greenPin 6
#define bluePin 11

#define christmasPin 2

// Servo set-up
#include <Servo.h>
#define servoPin 9
#define lockAngle 0
#define unlockAngle 113
Servo lockServo;


void setup()
{ 
  // The following line sets Timer 0 to Phase-Correct PWM Mode
  // to match Timer 2's Phase-Correct PWM Mode.
  // Timer 0 is also responsible for the internal timing
  // on the Arduino, so the delay(), millis(), and micros()
  // functions break when Timer 0 is in this mode.
  TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00); 

  // The following line sets Timer 2 to Fast PWM Mode to match
  // Timer 0's Fast PWM Mode.
  // This allows the internal timing of the Arduino to
  // continue working, letting delay(), etc. continue to work.
  // The downside to this is that the LEDs may flicker slightly.
  // TCCR2A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
  Serial.begin(115200);
  Serial.setTimeout(5);

  //configure pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(servoPin, OUTPUT);
  pinMode(christmasPin, OUTPUT);
  digitalWrite(christmasPin, LOW);

  lockServo.attach(servoPin);
  lockServo.write(lockAngle);
} 

void loop()
{
  switch (readData()) {
  case 0 :
    //Reset
    lockServo.write(lockAngle);
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    prevR = 0;
    prevG = 0;
    prevB = 0;
    redVal = black[0];
    grnVal = black[1]; 
    bluVal = black[2];
    break;
  case 1 :
    //LED Control
    //0-255 red; 0-255 green; 0-255 blue
    LEDControl(readData(), readData(), readData());
    break;
  case 2 :
    //Servo Control
    //0 == Lock; 1 == Unlock
    if (1){
      int lockCode = readData();
      if (lockCode == 0){
        lockServo.write(lockAngle);
      }
      else if (lockCode == 1){
        lockServo.write(unlockAngle);
      }
    }
    break;
  case 3 :
    //Crossfade
    //Fade red --> green --> blue, repeat
    prevR = 0;
    prevG = 0;
    prevB = 0;
    redVal = black[0];
    grnVal = black[1]; 
    bluVal = black[2];
    while (Serial.available() == 0)
    {
      crossFade(red);
      crossFade(green);
      crossFade(blue);
    }
    break;
  case 4 :
    //Christmas Mode!!
    //Fade black --> red --> black --> green, repeat
    prevR = 0;
    prevG = 0;
    prevB = 0;
    redVal = black[0];
    grnVal = black[1]; 
    bluVal = black[2];
    digitalWrite(christmasPin, HIGH);
    while (Serial.available() == 0)
    {
      crossFade(red);
      crossFade(black);
      crossFade(green);
      crossFade(black);
    }
    digitalWrite(christmasPin, LOW); 
    break;
  }
}


char readData() 
{
  Serial.println("w");
  while(1) {
    if(Serial.available() > 0) {
      return Serial.parseInt();
    }
  }
}

void LEDControl(int red, int green, int blue) 
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}


/* COLOR CROSSFADE STUFF!!! */

/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
 * 
 * The program works like this:
 * Imagine a crossfade that moves the red LED from 0-10, 
 *   the green from 0-5, and the blue from 10 to 7, in
 *   ten steps.
 *   We'd want to count the 10 steps and increase or 
 *   decrease color values in evenly stepped increments.
 *   Imagine a + indicates raising a value by 1, and a -
 *   equals lowering it. Our 10 step fade would look like:
 * 
 *   1 2 3 4 5 6 7 8 9 10
 * R + + + + + + + + + +
 * G   +   +   +   +   +
 * B     -     -     -
 * 
 * The red rises from 0 to 10 in ten steps, the green from 
 * 0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
 * 
 * In the real program, the color percentages are converted to 
 * 0-255 values, and there are 1020 steps (255*4).
 * 
 * To figure out how big a step there should be between one up- or
 * down-tick of one of the LED values, we call calculateStep(), 
 * which calculates the absolute gap between the start and end values, 
 * and then divides that gap by 1020 to determine the size of the step  
 * between adjustments in the value.
 */

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

/* The next function is calculateVal. When the loop value, i,
 *  reaches the step size appropriate for one of the
 *  colors, it increases or decreases the value of that color by 1. 
 *  (R, G, and B are each calculated separately.)
 */

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a 
 *  0-255 range, then loops 1020 times, checking to see if  
 *  the value needs to be updated each time, then writing
 *  the color values to the correct pins.
 */

void crossFade(int color[3]) {
  // Convert to 0-255
  int R = (color[0] * 255) / 100;
  int G = (color[1] * 255) / 100;
  int B = (color[2] * 255) / 100;

  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);

  for (int i = 0; i <= 1020; i++) {
    if (Serial.available() > 0) {
      break;
    }
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(redPin, redVal);   // Write current values to LED pins
    analogWrite(greenPin, grnVal);      
    analogWrite(bluePin, bluVal); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}


