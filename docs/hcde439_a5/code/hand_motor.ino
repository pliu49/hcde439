// Input: HC-SR04 ultrasonic distance sensor, read without a library. Output: a DC motor switched by an N-MOSFET (IRLZ44N) from an external 9 V battery.
// The closer my hand is to the sensor, the faster the motor spins (analogWrite PWM on the MOSFET gate).

// Pin wired to the sensor's TRIG pin: a 10 microsecond HIGH pulse starts one measurement
const int TRIG_PIN = 2;
// Pin wired to the sensor's ECHO pin: it is HIGH for as long as the sound needs to travel to my hand and back
const int ECHO_PIN = 3;
// PWM pin (marked ~ on the Uno) wired through gate resistor R2 (330 ohm) to the MOSFET gate
const int GATE_PIN = 9;
// Hand closer than this (in cm) gives full speed
const int NEAR_CM = 5;
// Hand farther than this (in cm) gives motor off
const int FAR_CM = 50;

// setup() runs once when the Arduino powers up or resets
void setup() {
  // Open the serial connection to the computer at 9600 baud
  Serial.begin(9600);
  // TRIG is an OUTPUT so the Arduino can send the trigger pulse
  pinMode(TRIG_PIN, OUTPUT);
  // ECHO is an INPUT (high impedance): the Arduino only listens to the sensor here
  pinMode(ECHO_PIN, INPUT);
  // Before this line the gate pin is an INPUT (high impedance, not driving anything), so the 10 kOhm pull-down R3 holds the gate at 0 V
  // pinMode(OUTPUT) makes the pin drive the gate; it starts LOW (0 V), so the MOSFET is off and the motor is stopped
  pinMode(GATE_PIN, OUTPUT);
}

// loop() runs over and over forever after setup() finishes
void loop() {
  // Start a measurement: TRIG goes HIGH ...
  digitalWrite(TRIG_PIN, HIGH);
  // ... for 10 microseconds ...
  delayMicroseconds(10);
  // ... and back to LOW
  digitalWrite(TRIG_PIN, LOW);
  // Measure how long ECHO stays HIGH, in microseconds; gives up after 30000 us (about 5 m) and returns 0
  long echoUs = pulseIn(ECHO_PIN, HIGH, 30000);
  // Without an echo (nothing in front of the sensor) assume the hand is far away
  int distanceCm = FAR_CM;
  // If an echo did come back
  if (echoUs > 0) {
    // Sound needs about 58 microseconds per cm for the round trip, so divide by 58 to get cm
    distanceCm = echoUs / 58;
  }
  // Map the distance to a speed: FAR_CM gives 0, NEAR_CM gives 255 (closer = faster)
  int speedValue = map(distanceCm, FAR_CM, NEAR_CM, 0, 255);
  // Keep the value between 0 and 255 so analogWrite gets a legal value (map() does not clamp)
  speedValue = constrain(speedValue, 0, 255);
  // Send the PWM duty cycle to the MOSFET gate: duty = speedValue / 255 of the time the gate is 5 V
  analogWrite(GATE_PIN, speedValue);
  // Print the label for the distance
  Serial.print("distance = ");
  // Print the distance in cm
  Serial.print(distanceCm);
  // Print the unit and the label for the PWM value
  Serial.print(" cm | analogWrite = ");
  // Print the analogWrite value and finish the line
  Serial.println(speedValue);
  // Wait 100 ms before the next measurement (about 10 per second)
  delay(100);
}
