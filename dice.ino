/*
 * This code is to build a single die with two tilt switches, six LEDs, and an ATTINY45.
 */

// Tilt sensor pin
#define UP_PIN 6

// Charliplexed LED Pins
#define P1 5
#define P2 2
#define P3 3
#define P4 4

const int pins[4] = {P1, P2, P3, P4};

// How many LEDs are there?
#define NUMPIPS 7

// Delay for charliplex. Cycles * delay is the 
// amount of time the value will show.
#define DELAY 1
#define CYCLES 1000

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
  0b01000000, // one
  0b00010010, // two
  0b01100001, // three
  0b00101101, // four
  0b01101101, // five
  0b00111111, // six
  0b01111111, // seven
};

#define OFF -1
const int pin_map[][4] = {
  {OFF, OFF, OFF, OFF},  // zero
  {OFF, OFF, LOW, HIGH}, // one
  {OFF, LOW, OFF, HIGH}, // two
  {LOW, OFF, OFF, HIGH}, // three
  {OFF, OFF, HIGH, LOW}, // four
  {OFF, HIGH, OFF, LOW}, // five
  {HIGH, OFF, OFF, LOW}, // six
  {OFF, LOW, HIGH, OFF}  // seven
};

void clear_pins() {
  for (int pid = 0; pid < 4; pid++) {
    
     // Look up the pin number in the array
     int pin = pins[pid];

     // Set all pins as input
      pinMode(pin, INPUT);
  }
}

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

/*
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
*/

void light_die(int val, int cycles=CYCLES) {
  // Light multiple pips for a die value

  byte pip_map = PIPS[val - 1];

  for (int j = 0; j < cycles; j++) {
    for (int pip = 0; pip < NUMPIPS; pip++) {

      // Shift the bit mask
      int mask = 1 << pip;

      
      if ((pip_map & mask) == mask) {
        light_pip(pip + 1);
        //Serial.println("Lighting pip+1");
      }
      delay(DELAY);
    }
    
  }

  clear_pins();
}

int old_state = 0;

void setup() {
  //start serial connection
  Serial.begin(9600);
  //randomSeed(analogRead(0));

  // Light each pip in sequence
  for (int p = 1; p <= 7; p++) {
    light_pip(p);
    delay(250); 
  }

  // Light each die value in sequence
  for (int d = 1; d <= 6; d++) {
    clear_pins();
    delay(100);
    light_die(d, 100);
  }

  // Blink all LEDs three times
  for (int i=0; i < 3; i++) {
    clear_pins();
    delay(100);
    light_die(7, 100);
    delay(100);
  }

  // Configure the two pins
  pinMode(UP_PIN, INPUT);

  // Add interrupts to call any time the pin values change
  // Using interrupts to help save some power
  //attachInterrupt(digitalPinToInterrupt(UP_PIN), check_state, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(DOWN_PIN), check_state, CHANGE);  
}

void check_state() {
  
  // Obtain the new state from the pins
  int new_state = digitalRead(UP_PIN);

  if (new_state != old_state) {

    if (new_state) {
      light_die(random(1, 7));
    }

    old_state = new_state;
  }
}

void loop() {
  //light_die(random(1, 7));
  check_state();
  delay(100);
  
}





