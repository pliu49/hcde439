// HCDE 439 Assignment 4: Touch-controlled servo (two libraries: Servo and CapacitiveSensor).
// Input: touching a foil pad (capacitive touch). Output: a servo that opens while the pad is touched and closes when it is released.

// Servo library (comes with the Arduino IDE): makes the control pulses that position the servo
#include <Servo.h>
// CapacitiveSensor library (install from Library Manager): measures how much a touch changes the pad's capacitance
#include <CapacitiveSensor.h>

// Digital pin that sends the pulse through resistor R1 to the pad
const uint8_t SEND_PIN = 2;
// Digital pin that receives the pulse and is also wired to the touch pad
const uint8_t RECEIVE_PIN = 4;
// Digital pin wired to the servo's orange signal wire
const uint8_t SERVO_PIN = 9;
// Servo angle in degrees when nothing touches the pad (blinds closed)
const int CLOSED_ANGLE = 0;
// Servo angle in degrees while the pad is touched (blinds open)
const int OPEN_ANGLE = 180;
// Readings averaged per measurement: 30 gives a steady value and is the number used in the class demo
const uint8_t SAMPLES = 30;
// A reading above this counts as a touch: my readings were about 8 to 11 untouched and 249 touched, so 100 sits between them
const long TOUCH_THRESHOLD = 100;
// Milliseconds to pause after each reading: with the sensor's own read time this gives about 15 readings per second, fast enough for a touch without flooding the Serial Monitor
const int LOOP_DELAY_MS = 50;

// Create the touch sensor object on the send and receive pins
CapacitiveSensor touchPad = CapacitiveSensor(SEND_PIN, RECEIVE_PIN);
// Create the servo object that represents the motor
Servo myServo;

// setup() runs once when the Arduino powers up or resets
void setup() {
  // Open the serial connection to the computer at 9600 baud
  Serial.begin(9600);
  // Tell the Servo library which pin carries the control signal
  myServo.attach(SERVO_PIN);
  // Start with the servo closed
  myServo.write(CLOSED_ANGLE);
  // Remind me not to touch the pad now, because the library treats the first readings as "no touch"
  Serial.println("Do not touch the pad while the Arduino starts: the library takes its zero point from the first readings.");
}

// loop() runs over and over forever after setup() finishes
void loop() {
  // Measure the pad: bigger number = more capacitance = a finger is closer
  long reading = touchPad.capacitiveSensor(SAMPLES);
  // Decide whether the reading is large enough to be a touch
  bool touched = reading > TOUCH_THRESHOLD;
  // Move the servo to the open angle when touched, otherwise to the closed angle
  myServo.write(touched ? OPEN_ANGLE : CLOSED_ANGLE);
  // Print the time, the reading and what the servo was told to do
  Serial.println(String("Touch servo | t=") + millis() + " ms | reading=" + reading + " | " + (touched ? "TOUCHED: open" : "not touched: closed"));
  // Wait before the next pass
  delay(LOOP_DELAY_MS);
}
