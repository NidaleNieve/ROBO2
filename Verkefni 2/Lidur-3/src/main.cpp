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

#include <cstdlib> //exit library
#include "vex.h"

using namespace vex;
}

int gradur = 360; //breyta þessu til þess að velja gráðurnar sem vélmenninð keyrir í hring

int checkEmergencyStop() {
  while(true) {
    if (Controller1.ButtonX.pressing()) {
      LeftMotor.stop();
      RightMotor.stop();
      Brain.Screen.printAt(10, 50, "EMERGENCY STOP PRESSED!");

      wait(50, msec);
      exit(0);
    }
    wait(20, msec);
  }
}

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  thread t1 = thread(checkEmergencyStop);

  Controller1.ButtonX.pressed(emergencyStop);

  //Basic loopa sem fer í gegnum gradur var
  for (int x = 0; x < gradur; x++) {
    LeftMotor.setVelocity(20, percent);
    RightMotor.setVelocity(40, percent);
    LeftMotor.spin(forward);
    RightMotor.spin(forward);

    wait(25, msec);
  }
  LeftMotor.stop();
  RightMotor.stop();
}
