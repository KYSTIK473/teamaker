#include "pcf8574.h"
#include "CustomStepper.h"

PCF8574 pcf(0x20);
CustomStepper stepper(pcf, 0, 1, 2, 3);
CustomStepper stepper2(pcf, 4, 5, 6, 7);
void setup() {
  // put your setup code here, to run once:
  stepper.setRPM(6);
  stepper.setSPR(4075.7728395);
  stepper2.setRPM(6);
  stepper2.setSPR(4075.7728395);
  //stepper.disable();
}

void loop() {
  
  // put your main code here, to run repeatedly:
  stepper.setDirection(CW);
  //this method sets the motor to rotate a given number of times, if you don't specify it, 
  //the motor will rotate untilyou send another command or set the direction to STOP
  stepper.rotateDegrees(180);
  stepper2.setDirection(CCW);
  //this method sets the motor to rotate a given number of times, if you don't specify it, 
  //the motor will rotate untilyou send another command or set the direction to STOP
  stepper2.rotateDegrees(180);
  stepper2.run();
  stepper.run();
}
