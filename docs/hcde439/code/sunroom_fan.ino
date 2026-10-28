// HCDE 439 Assignment 3: Sunroom fan. A photoresistor voltage divider sets the speed of a DC motor fan.
// Divider: +5V -> R1 (photoresistor) -> A0 node -> R2 (10 kOhm) -> GND, so more light means a higher voltage on A0.

// Analog input pin wired to the divider's V_measure node (between the photoresistor and the 10 kOhm resistor)
const int LIGHT_PIN = A0;
// PWM-capable output pin wired through the 1 kOhm base resistor R3 to the base of transistor Q1
const int MOTOR_PIN = 9;
// Reading below this means the room is too dark to need cooling: 250 is about 1.22 V, which is a photoresistor of about 31 kOhm
const int DARK_THRESHOLD = 250;
// Brightest reading expected in my room: 850 is about 4.15 V, which is a photoresistor of about 2 kOhm
const int LIGHT_MAX = 850;
// Lowest PWM duty that reliably starts my motor: 80 out of 255 is about 31 percent
const int PWM_MIN = 80;
// Highest value analogWrite() accepts (8-bit), which is 100 percent duty and full fan speed
const int PWM_MAX = 255;
// Milliseconds to wait between samples, 200 ms gives 5 readings per second
const int SAMPLE_DELAY_MS = 200;

// setup() runs once when the Arduino powers up or resets
void setup() {
  // Open the serial connection to the computer at 9600 baud so I can watch the readings
  Serial.begin(9600);
  // Make the motor pin an output so it can drive the transistor base
  pinMode(MOTOR_PIN, OUTPUT);
}

// loop() runs over and over forever after setup() finishes
void loop() {
  // Read the divider voltage as a 10-bit number from 0 (0 V) to 1023 (5 V)
  int lightValue = analogRead(LIGHT_PIN);
  // Convert the 0-1023 reading back into volts so I can compare it with my hand calculations
  float volts = lightValue * 5.0 / 1023.0;
  // Variable that will hold the 8-bit PWM value (0-255) sent to the motor
  int fanSpeed;

  // Check whether the room is darker than my threshold
  if (lightValue < DARK_THRESHOLD) {
    // Too dark to need the fan, so switch the motor fully off
    fanSpeed = 0;
  // Otherwise the room is bright enough that the fan should run
  } else {
    // Scale the light reading (250 to 850) to a motor PWM value (80 to 255)
    fanSpeed = map(lightValue, DARK_THRESHOLD, LIGHT_MAX, PWM_MIN, PWM_MAX);
    // Clamp the result because map() does not limit values outside the input range (readings above 850)
    fanSpeed = constrain(fanSpeed, PWM_MIN, PWM_MAX);
  }

  // Send the PWM value to the transistor base, which sets the fan speed
  analogWrite(MOTOR_PIN, fanSpeed);

  // Start the serial line with a label and the time since power-up so I can graph voltage against time later
  Serial.print("Sunroom fan | t=");
  // Print the milliseconds since the Arduino started
  Serial.print(millis());
  // Print a separator and label for the raw ADC reading
  Serial.print(" ms | A0=");
  // Print the raw 0-1023 reading
  Serial.print(lightValue);
  // Print a separator and label for the voltage at V_measure
  Serial.print(" | V=");
  // Print the voltage rounded to two decimal places
  Serial.print(volts, 2);
  // Print a separator and label for the PWM value
  Serial.print(" V | PWM=");
  // Print the 0-255 value sent to the motor
  Serial.print(fanSpeed);
  // Print the fan state and end the line with a newline
  Serial.println(fanSpeed == 0 ? " | fan OFF (dark)" : " | fan ON");

  // Wait before the next reading so the serial monitor is readable and the divider has time to settle
  delay(SAMPLE_DELAY_MS);
}
