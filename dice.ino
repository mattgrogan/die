/*
 * This code is to build a single die with two tilt switches, six LEDs, and an ATTINY45.
 */

// Orientation constants
const int UP = 1;
const int DOWN = 2;

// Tilt sensor pins
const int UP_PIN = 3;
const int DOWN_PIN = 2;

// Charliplexed LED Pins
#define P1 8
#define P2 9
#define P3 10
#define P4 11

const int pins[4] = {P1, P2, P3, P4};

// How many LEDs are there?
#define NUMPIPS 7

// Delay for charliplex. Cycles * delay is the 
// amount of time the value will show.
#define DELAY 1
#define CYCLES 1000

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

void light_die(int val, int cycles=CYCLES) {
  // Light multiple pips for a die value

  byte pip_map = PIPS[val - 1];

  for (int j = 0; j < cycles; j++) {
    for (int pip = 0; pip < NUMPIPS; pip++) {

      // Shift the bit mask
      int mask = 1 << pip;
      
      if ((pip_map & mask) == mask) {
        light_pip(pip + 1);
      }
      delay(DELAY);
    }
    
  }

  clear_die();
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

  // Blink all LEDs three times
  for (int i=0; i < 3; i++) {
    light_die(7, 50);
    delay(75);
  }

  // Configure the two pins
  pinMode(UP_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);

  // Add interrupts to call any time the pin values change
  // Using interrupts to help save some power
  attachInterrupt(digitalPinToInterrupt(UP_PIN), check_state, CHANGE);
  attachInterrupt(digitalPinToInterrupt(UP_PIN), check_state, CHANGE);  
}

void check_state() {
  
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
}

void loop() {
  // Nothing to do
}





