/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Tue Oct 01 2019                                           */
/*    Description:  Clawbot Control                                           */
/*                  This program uses Controller events to drive the arm and  */
/*                  claw of the V5 Clawbot.                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// LeftMotor            motor         1               
// RightMotor           motor         10              
// ClawMotor            motor         3               
// ArmMotor             motor         8               
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

bool clawOpenDirectionIsForward = false; // false = opening (reverse), true = closing (forward)
bool armUpDirectionIsForward = true;     // true = lifting (forward), false = lowering (reverse)

// Declare Contoller event callbacks.
void whenControllerL1Pressed() {
  // Toggle arm direction on each press
  if (armUpDirectionIsForward) {
    ArmMotor.spin(forward);
  } else {
    ArmMotor.spin(reverse);
  }
  // Run while the button is held, stop on release
  waitUntil(!Controller1.ButtonL1.pressing());
  ArmMotor.stop();
  // Flip direction for next press
  armUpDirectionIsForward = !armUpDirectionIsForward;
}

void whenControllerR1Pressed() {
  // Toggle claw direction on each press
  if (clawOpenDirectionIsForward) {
    // Closing direction
    ClawMotor.spin(forward);
  } else {
    // Opening direction
    ClawMotor.spin(reverse);
  }
  // Run while the button is held, stop on release
  waitUntil(!Controller1.ButtonR1.pressing());
  ClawMotor.stop();
  // Flip direction for next press
  clawOpenDirectionIsForward = !clawOpenDirectionIsForward;
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  
  // Initialize the Contoller Events
  Controller1.ButtonL1.pressed(whenControllerL1Pressed);
  Controller1.ButtonR1.pressed(whenControllerR1Pressed);

  // Set the brake mode and velocity of the ArmMotor and ClawMotor
  ClawMotor.setStopping(hold);
  ArmMotor.setStopping(hold);

  ClawMotor.setVelocity(100, percent);
  ArmMotor.setVelocity(60, percent);

  // Use tank drive to control the robot.
  while (true) {
    LeftMotor.setVelocity(Controller1.Axis3.position(), percent);
    RightMotor.setVelocity(Controller1.Axis2.position(), percent);
    LeftMotor.spin(forward);
    RightMotor.spin(forward);
    
    wait(25, msec);
  }
  
}
