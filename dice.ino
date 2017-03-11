/*
 * This code is to build a single die with a tilt switch, seven LEDs, and an ATTINY45.
 */

#include <avr/interrupt.h>
#include <avr/sleep.h>

// Tilt sensor pin
#define TILT_PIN 0

// Charliplexed LED Pins
#define P1 1
#define P2 2
#define P3 3
#define P4 4
const int pins[4] = {P1, P2, P3, P4};

// How many LEDs are there?
#define NUMPIPS 7

// Delay for charliplex. Cycles * delay is the 
// amount of time the value will show.
#define DELAY 0
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

// Which pips are lit for each die roll?
const byte PIPS[] = {
  0b01000000, // one
  0b00010010, // two
  0b01100001, // three
  0b00101101, // four
  0b01101101, // five
  0b00111111, // six
  0b01111111, // seven
};

//State definitions
#define ROLL_DISABLED     0
#define ROLL_WAITING      1
#define ROLL_TRIGGERED    2

//State variable
volatile uint8_t state;

void clear_pins() {
  // Reset all pins
  for (int pid = 0; pid < 4; pid++) {
    
     // Look up the pin number in the array
     int pin = pins[pid];

     // Set all pins as input
      pinMode(pin, INPUT);
  }
}

void light_pip(int pip) {
  // Light a single pip on the die

  clear_pins();

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

void cycle_pips() {
  // Light each pip in sequence
  for (int p = 1; p <= 7; p++) {
    light_pip(p);
    delay(100); 
  }
}

void cycle_vals() {
  // Light each die value in sequence
  for (int d = 1; d <= 6; d++) {
    clear_pins();
    delay(100);
    light_die(d, 50);
  }  
}

void blink_all(int n) {
  // Blink all LEDs n times
  for (int i=0; i < n; i++) {
    clear_pins();
    delay(100);
    light_die(7, 100);
    delay(100);
  }
}

void setup() {
  
  randomSeed(micros);

  cycle_pips();
  delay(100);
  cycle_vals();

  // Configure the input pin
  pinMode(TILT_PIN, INPUT);

  // Start with the state enabled
  state = ROLL_WAITING;

  // Using interrupts to help save some power
  // Using interrupt example from:
  // https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/

  GIMSK = 0b00100000;
  PCMSK = 0b00000001;
  sei();
}

ISR(PCINT0_vect) {
  // set the roll boolean to true, so it will
  // be picked up on the next loop.

  if (state == ROLL_WAITING) {
    state = ROLL_TRIGGERED;
  }
}

void loop() {

  if (state == ROLL_TRIGGERED) {
    // Clear the state
    state = ROLL_DISABLED;

    light_die(random(1, 7));
    delay(100);

    // Wait for the next roll
    state = ROLL_WAITING;
  }

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  if (state != ROLL_TRIGGERED) {
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  }
  sei();
}





