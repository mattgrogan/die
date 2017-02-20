/*
 Input Pullup Serial

 This example demonstrates the use of pinMode(INPUT_PULLUP). It reads a
 digital input on pin 2 and prints the results to the serial monitor.

 The circuit:
 * Momentary switch attached from pin 2 to ground
 * Built-in LED on pin 13

 Unlike pinMode(INPUT), there is no pull-down resistor necessary. An internal
 20K-ohm resistor is pulled to 5V. This configuration causes the input to
 read HIGH when the switch is open, and LOW when it is closed.

 created 14 March 2012
 by Scott Fitzgerald

 http://www.arduino.cc/en/Tutorial/InputPullupSerial

 This example code is in the public domain

 */
const int UP = 1;
const int DOWN = 2;

const int UP_PIN = 3;
const int DOWN_PIN = 2;

int old_state = 0;

int read_orientation() {
  // Read the tilt sensor value and determine the orientation
  int up = digitalRead(UP_PIN) << 1; // Bitshift by one
  int down = digitalRead(DOWN_PIN);

  return up + down;
}

void setup() {
  //start serial connection
  Serial.begin(9600);

  // Configure the two pins
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
}

void loop() {

  int new_state = read_orientation();

  if (new_state != old_state) {
    switch(new_state) {
      case UP:
        Serial.println("UP");
        break;
       case DOWN:
        Serial.println("DOWN");
        break;
       default:
        Serial.println("SIDEWAYS");
        break;
    }
    old_state = new_state;
  }

  delay(100);


}



