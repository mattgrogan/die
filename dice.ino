/*
 * This code is to build a single die with two tilt switches, six LEDs, and an ATTINY45.
 */

const int UP = 1;
const int DOWN = 2;

// Tilt sensors
const int UP_PIN = 3;
const int DOWN_PIN = 2;

#define P1 8
#define P2 9
#define P3 10
#define P4 11

const int pins[4] = {P1, P2, P3, P4};

// How many LEDs are there?
#define NUMPIPS 7

// Delay for charliplex
#define DELAY 1

// Delay between number display
#define PAUSE 100

// Which pips are lit for each die roll 1..6?
/*
 *    --------- 
 *    | 1   4 |
 *    | 2 7 5 |
 *    | 3   6 |
 *    ---------
 * 
 */
const byte PIPS[] = {
  0b01000000,
  0b00010010,
  0b01100001,
  0b01001101,
  0b01101101,
  0b00111111,
  0b01111111,
};

#define OFF -1
const int pin_map[][4] = {
  {OFF, OFF, OFF, OFF},
  {LOW, HIGH, OFF, OFF},
  {HIGH, LOW, OFF, OFF},
  {OFF, OFF, HIGH, LOW},
  {OFF, OFF, LOW, HIGH},
  {OFF, LOW, HIGH, OFF},
  {OFF, HIGH, LOW, OFF},
  {HIGH, OFF, LOW, OFF}
};

void light_pip(int pip) {
  // Light a single pip on the die

  for (int pid = 0; pid < 4; pid++) {

    // Should the pin be high, low, or off?
    int pin_value = pin_map[pip][pid];

    // Look up the pin number in the array
    int pin = pins[pid];
    
    switch(pin_value) {
      case HIGH:
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        break;
      case LOW:
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        break;
      default:
        pinMode(pin, INPUT);
        break;
    }
  }
}

void clear_die() {
    // Turn off all pips
  for (int pip = 0; pip < NUMPIPS; pip++) {
    for (int pid = 0; pid < 4; pid++) {
  
 
      // Look up the pin number in the array
      int pin = pins[pid];
      digitalWrite(pin, LOW);
      pinMode(pin, INPUT);
    }
  }
}

void light_die(int val) {
  // Light multiple pips for a die value

  byte pip_map = PIPS[val - 1];

  for (int j = 0; j < 1000; j++) {
    for (int pip = 0; pip < NUMPIPS; pip++) {

      // Shift the bit mask
      int mask = 1 << pip;
      
      if ((pip_map & mask) == mask) {
        light_pip(pip + 1);
        delay(DELAY);
      }
    }
    
  }

  clear_die();
}

void test_die() {
  // Test all the LEDs on the die
  light_die(7); 
}


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
  randomSeed(analogRead(0));

  // Configure the two pins
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);

  // Test the die
  test_die();
  delay(1000);
}

void loop() {

  // Obtain the new state from the pins
  int new_state = read_orientation();

  if (new_state != old_state) {
    // Only output if the state has changed
    switch(new_state) {
      case UP:
        light_die(random(1, 7));
        break;
       case DOWN:
        //Serial.println("DOWN");
        break;
       default:
        //Serial.println("SIDEWAYS");
        break;
    }
    old_state = new_state;
  }

  delay(100);
}





