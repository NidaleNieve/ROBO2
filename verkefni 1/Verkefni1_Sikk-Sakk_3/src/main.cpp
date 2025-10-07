/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Wed Dec 04 2019                                           */
/*    Description:  This program will turn right 90 degrees using the         */
/*                  Inertial Sensor.                                          */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// LeftMotor            motor         1               
// RightMotor           motor         10              
// Inertial20           inertial      20              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
using namespace vex;
#include <algorithm>
#include <cmath> 

/* Kóði búinn til af ChatGPT sem virkar vel en er alltof erfiður
// Helper: sign
static inline double sgn(double x){ return (x>0)-(x<0); }

// beygir hlutfallslega um tilteknar gráður (jákvætt = hægri, neikvætt = vinstri)
void turnDelta(double deltaDeg) {

    // --- Stillingar ---
  static const double kP                 = 0.9;   // proportional gain (power per deg of error)
  static const double kMinPower          = 8.0;   // minimum % power so motors actually move
  static const double kMaxPower          = 40.0;  // safety cap
  static const double kSlowdownThreshold = 35.0;  // start gentle power within this many degrees
  static const double kTolerance         = 1.0;   // consider “on target” within ± this many degrees
  static const double kSettleTime        = 120;   // ms: must stay within tolerance this long
  static const int    kLoopDtMs          = 10;    // control loop period
  static const int    kTimeoutMsPer90    = 2000;  // general timeout scale (≈2s per 90°)


  // choose a firm stop to reduce coasting past the target
  LeftMotor.setStopping(brake);
  RightMotor.setStopping(brake);

  const double start = Inertial20.rotation(degrees);
  const double target = start + deltaDeg;

  // simple P control with min power + cap + slowdown
  int elapsed = 0;
  int withinStart = -1;

  // total timeout proportional to requested turn
  const int hardTimeout = std::max(700, (int)(std::abs(deltaDeg) * (kTimeoutMsPer90 / 90.0)));

  while (elapsed < hardTimeout) {
    double angle  = Inertial20.rotation(degrees);
    double error  = target - angle;
    double aerr   = std::abs(error);

    if (aerr <= kTolerance) {
      if (withinStart < 0) withinStart = elapsed;             // just entered the window
      if (elapsed - withinStart >= kSettleTime) break;        // stayed long enough
    } else {
      withinStart = -1;                                       // left the window
    }

    // proportional term
    double power = kP * error;

    // gentle power near the end: compress power inside slowdown zone
    if (aerr < kSlowdownThreshold) {
      power *= (aerr / kSlowdownThreshold);                   // linear taper
    }

    // enforce min power so it doesn’t stall, but keep direction
    if (std::abs(power) < kMinPower) power = sgn(power) * kMinPower;

    // cap max power
    if (std::abs(power) > kMaxPower) power = sgn(power) * kMaxPower;

    // apply tank turn: right error => turn right (left reverse, right forward)
    LeftMotor.spin(reverse, power, percent);
    RightMotor.spin(forward,  power, percent);

    wait(kLoopDtMs, msec);
    elapsed += kLoopDtMs;
  }

  // stop with brake to avoid roll-past
  LeftMotor.stop();
  RightMotor.stop();

  // --- auto-trim phase (very low power) in case we still missed by a hair ---
  double finalErr = (start + deltaDeg) - Inertial20.rotation(degrees);
  if (std::abs(finalErr) > kTolerance) {
    const double trimPower = 6.0 * sgn(finalErr);             // tiny nudge
    const int    trimTimeout = 400;                           // ms
    int t = 0;
    while (std::abs(finalErr) > kTolerance && t < trimTimeout) {
      LeftMotor.spin(reverse, trimPower, percent);
      RightMotor.spin(forward,  trimPower, percent);
      wait(20, msec);
      t += 20;
      finalErr = (start + deltaDeg) - Inertial20.rotation(degrees);
    }
    LeftMotor.stop();
    RightMotor.stop();
  }

  wait(300, msec); // brief settle
}

void turnRight90() {
  Brain.Screen.print("Beygir til hægri");
  Brain.Screen.clearScreen();

  turnDelta(90.0); //keyri Delta fallið sem beygir mjög nákvæmlega
}

void turnLeft90() {
  Brain.Screen.print("Beygir til vinstri");
  Brain.Screen.clearScreen();

  turnDelta(-90.0); //keyri Delta fallið sem beygir mjög nákvæmlega
}
*/

void driveForward50cm() {
  Brain.Screen.print("Keyri áfram");
  Brain.Screen.clearScreen();

  LeftMotor.spin(forward, 30, percent);
  RightMotor.spin(forward, 30, percent);
  wait(1.5, seconds);

  LeftMotor.stop();
  RightMotor.stop();

  wait(1, seconds);
}

void turnRight90() {
  Brain.Screen.print("Beygir til hægri");
  Brain.Screen.clearScreen();

  double startAngle = Inertial20.rotation(degrees);
  LeftMotor.spin(reverse, 20, percent);
  RightMotor.spin(forward, 20, percent);

  waitUntil(Inertial20.rotation(degrees) >= startAngle + 90.0);
  LeftMotor.stop();
  RightMotor.stop();

  wait(0.3, seconds);
}

void turnLeft90() {
  Brain.Screen.print("Beygir til vinstri");
  Brain.Screen.clearScreen();

  double startAngle = Inertial20.rotation(degrees);
  LeftMotor.spin(forward, 15, percent);
  RightMotor.spin(reverse, 15, percent);

  waitUntil(Inertial20.rotation(degrees) <= startAngle - 90.0);
  LeftMotor.stop();
  RightMotor.stop();

  wait(0.3, seconds);
}



int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  Inertial20.calibrate();
  // waits for the Inertial Sensor to calibrate
  while (Inertial20.isCalibrating()) {
    wait(100, msec);
  }

  //Aðal forritið
  for (int var = 1, moves = 0; moves <= 11;) { //stilli upp breytur
    driveForward50cm();

    if (var == 7) { //snúi við
      turnRight90();

      driveForward50cm();

      turnRight90();

      driveForward50cm();

      var = 11; //set varið á 11 þannig þetta if fer ekki í gang aftur
    }

    //Aðal if fallið, sér til um í hvaða átt að beygja
    if (var % 4 == 1 || var % 4 == 0) {

      turnRight90();
    } else {
      turnLeft90();
    }
    
    var += 1;
    moves += 1; //bæti við moves til að vita hvenær vélmennið er komið
  }
  
    driveForward50cm(); //keyri síðustu 50cm 
}
