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

// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

#pragma endregion VEXcode Generated Robot Configuration

int Screen_precision = 0, Console_precision = 0, Controller1_precision = 0;

float DrivingDistance;

bool EmergencyStop;

event LightDisplay = event();
event SpeedDisplay = event();

//Emergency Stop triggers
void onevent_Controller1ButtonX_pressed_0() {
  EmergencyStop = true;
}
void onevent_BumperH_pressed_0() {
  EmergencyStop = true;
}

// "when started" hat block
int whenStarted1() {

}

// Used to find the format string for printing numbers with the
// desired number of decimal places
const char* printToScreen_numberFormat() {
  // look at the current precision setting to find the format string
  switch(Screen_precision){
    case 0:  return "%.0f"; // 0 decimal places (1)
    case 1:  return "%.1f"; // 1 decimal place  (0.1)
    case 2:  return "%.2f"; // 2 decimal places (0.01)
    case 3:  return "%.3f"; // 3 decimal places (0.001)
    default: return "%f"; // use the print system default for everything else
  }
}

//prentar hraða og vegalengd
void speedDisplay() {
  Brain.Screen.clearLine(1);
  Brain.Screen.setCursor(Brain.Screen.row(), 1);
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Velocity:  ");

  //Brain.Screen.print(printToScreen_numberFormat(), static_cast<float>((Drivetrain.velocity(rpm) / 60.0) * (6.28318530718 * 0.05)));
  double wheelRadius = 0.05; // meters
  double drivetrainRPM = Drivetrain.velocity(velocityUnits::rpm);
  double velocityMS = (drivetrainRPM / 60.0) * (2 * 3.14159265359 * wheelRadius);
  Brain.Screen.print(velocityMS);

  Brain.Screen.print("m/s   Distance:  ");
  //DrivingDistance = DrivingDistance + ((Brain.Timer.time(seconds)) * (Drivetrain.velocity(rpm) / 60.0)) * (6.28318530718 * 0.05);
  //Brain.Screen.print(printToScreen_numberFormat(), static_cast<float>(DrivingDistance));
  Brain.Screen.print(Drivetrain.distance(distanceUnits::m));
  Brain.Screen.print("m");
}

//prenta ljósalevel
void lightDisplay() {
  Brain.Screen.clearLine(2);
  Brain.Screen.setCursor(Brain.Screen.row(), 1);
  Brain.Screen.setCursor(2, 1);
  Brain.Screen.print("Light Level:  ");
  Brain.Screen.print(printToScreen_numberFormat(), static_cast<float>(LightC.brightness()));
}


int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  // starta event handlers, fyrir emergency stop 
  Controller1.ButtonX.pressed(onevent_Controller1ButtonX_pressed_0);
  BumperH.pressed(onevent_BumperH_pressed_0);
  // event handlers fyrir display
  SpeedDisplay(speedDisplay);
  LightDisplay(lightDisplay);

  wait(15, msec);

  //aðal forritið
  DrivingDistance = 0.0;
  EmergencyStop = false;
  Drivetrain.setTurnVelocity(50.0, percent);
  Drivetrain.setDriveVelocity(20.0, percent);
  // Færi Arm upp þannig hann sé ekki fyrir
  Motor8.setVelocity(20.0, percent);
  Motor8.spinToPosition(120.0, degrees, true);

  while ((!EmergencyStop)) {
    // Ljósaskynjari. Keyrir bara ef að ljós > 70
    if (LightC.brightness() > 70.0) {
      LightDisplay.broadcast();
      // sonar skynjari. Stoppar ef að finnur eitthvað nálægt
      if (RangeFinderE.distance(mm) < 700.0) {
        Brain.Timer.clear();
        wait(0.02, seconds);
        Drivetrain.turn(right);
      }
      else {
        Brain.Timer.clear();
        Drivetrain.setDriveVelocity(30.0, percent);
        Drivetrain.drive(forward);
      }
      SpeedDisplay.broadcast();
      wait(0.02, seconds);
    }
    else {
      Brain.Timer.clear();
      Drivetrain.stop();
      wait(0.02, seconds);
    }
  wait(5, msec);
  }
  Motor8.stop();
  Drivetrain.stop();
  Brain.programStop();
  return 0;
}