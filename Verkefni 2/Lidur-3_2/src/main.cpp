/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Wed Sep 25 2019                                           */
/*    Description:  Moving Forward (mm)                                       */
/*                                                                            */
/*    This Program drives the robot forward for 150 millimeters.              */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, D
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

using namespace vex;

int gradur = 180; //breyta þessu til þess að velja gráðurnar sem vélmenninð keyrir í hring

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  motor_group leftDrive(LeftDriveSmart);
  motor_group rightDrive(RightDriveSmart);

  leftDrive.setVelocity(50, percent);
  rightDrive.setVelocity(45, percent);

  leftDrive.spin(forward);
  rightDrive.spin(forward);

  Drivetrain.turnToHeading(gradur + 10, degrees);
}
