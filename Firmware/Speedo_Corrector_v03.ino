/*----------------------------------------------------------------------------
 Speedo_Corrector_v03.ino

 Speedometer corrector for Jeep Wrangler TJ (1997-2006).

 Based on Tom M'Guinness's Speedometer Calibrator for the 1986 Porsche 944
 Turbo.  https://www.carpokes.com/viewtopic.php?t=558

 This work is licensed under a Creative Commons Attribution-NonCommercial 4.0
 International License (cc-by-nc), a copy of which can be found here:
 https://creativecommons.org/licenses/by-nc/4.0/

 Version:
 (v0.1, 0.2, 0.3 modified by OldBuzzard)
 0.1  2022/12/03  Modified very slightly from Tom's original code, mainly to
                  add DIP switch input for the calibration factor.
 0.2  2023/01/29  Added a watchdog timer and a check for >99% delta.
 0.3  2023/02/09  Changed var names to debug easier, changed calFactor calcs,
                  still using 98% Tom-code.
----------------------------------------------------------------------------*/

#include<avr/wdt.h>    // Header for watchdog timers in AVR 

const int              debounce = 2500;
const int              speedometerPin = 4; // Ard D4 (MCU pin 2) digital out
const int              sensorPin = 3;      // Ard D3 (MCU pin 1) dig input/int
int                    pulseState = LOW;
unsigned long          loopCurrentTime = 0;
unsigned long          loopPreviousTime = 0;
volatile unsigned long isrCurrentTime = 0;
volatile unsigned long isrPreviousTime = 0;
volatile unsigned long isrInterval = 0;
unsigned long          loopInterval = 0;
unsigned long          loopIsrPreviousTime = 0;
float                  calFactor = .91;    // decrease to slow down speedometer
                                           // calFactor=1 = no change to speedo

/*----------------------------------------------------------------------------
 Interrupt service routine (input pin falling edge)

 Measures the interval, in microseconds, between falling edges of the input
 signal. Therefore, the returned "isrInterval" is the period of a full cycle.
----------------------------------------------------------------------------*/
void iSr()
{
  // uS since power-on, resolution 4uS, unsigned long (32 bit integer)
  isrCurrentTime = micros();

  // the only way to get into this ISR is on a falling edge, not sure this
  // if() is really needed, might act as a sorta-debounce if the signal has
  // gone back high before we got here, would have to wait for another int
  if (digitalRead(sensorPin) == LOW)
  {
    if ((isrCurrentTime - isrPreviousTime) > debounce) // ok on rollover
    {
      isrInterval = isrCurrentTime - isrPreviousTime; // ok on rollover
      isrPreviousTime = isrCurrentTime;  // remember this time
    }
  }
}

/*----------------------------------------------------------------------------
 Arduino setup
----------------------------------------------------------------------------*/
void setup()
{
  int     iAmount = 0;                // amount to increase or decrease
  bool    bSpdIncrease;               // 1=increase, 0=decrease

  pinMode (5, INPUT_PULLUP);          // Arduino D5 (8-DIP-switch LSB)
  pinMode (6, INPUT_PULLUP);          // Arduino D6 (8-DIP-switch)
  pinMode (7, INPUT_PULLUP);          // Arduino D7 (8-DIP-switch)
  pinMode (8, INPUT_PULLUP);          // Arduino D8 (8-DIP-switch)
  pinMode (9, INPUT_PULLUP);          // Arduino D9 (8-DIP-switch)
  pinMode (10, INPUT_PULLUP);         // Arduino D19 (8-DIP-switch)
  pinMode (11, INPUT_PULLUP);         // Arduino D11 (8-DIP-switch MSB)
  pinMode (12, INPUT_PULLUP);         // Arduino D12 (8-DIP-switch increase)

  pinMode (13, OUTPUT);               // on-board LED
  pinMode(speedometerPin, OUTPUT);    // out to speedometer
  pinMode(sensorPin, INPUT);          // in from speed sensor
  attachInterrupt (1, iSr, FALLING);  // MCU pin 1, INT1 (Arduino D3)

  // read in 8-DIP-switch
  bitWrite(iAmount, 0, digitalRead(5));   // LSB
  bitWrite(iAmount, 1, digitalRead(6));
  bitWrite(iAmount, 2, digitalRead(7));
  bitWrite(iAmount, 3, digitalRead(8));
  bitWrite(iAmount, 4, digitalRead(9));
  bitWrite(iAmount, 5, digitalRead(10));
  bitWrite(iAmount, 6, digitalRead(11));  // MSB
  if(iAmount > 99) iAmount = 99;          // set a limit
  bSpdIncrease = digitalRead(12);         // 1=increase, 0=decrease

  // set calFactor
  if(bSpdIncrease) {    // increase
    //calFactor = 1 + ((float)iAmount / 100.0);
    calFactor = 1 - ((float)iAmount / 100.0);
  } else {              // decrease
    //calFactor = 1 - ((float)iAmount / 100.0);
    calFactor = 1 + ((float)iAmount / 100.0);
  }

  wdt_enable(WDTO_1S);  // Enable the watchdog with a timeout of 1 second
}

/*----------------------------------------------------------------------------
 Arduino main loop
----------------------------------------------------------------------------*/
void loop()
{
  noInterrupts();
    loopInterval = isrInterval;             // get values set
    loopIsrPreviousTime = isrPreviousTime;  //   by ISR
  interrupts();

  // uS since power-on, resolution 4uS, unsigned long (32 bit integer)
  loopCurrentTime = micros();

  // if the time since the last input pulse is 1 second or longer, don't
  //   send any signals (we're below 0.5 MPH):
  if ((loopCurrentTime - loopIsrPreviousTime) < 1000000)
  {
    // if it's been over 1/2 a cycle time * the calibration factor:
    //               original code said:       ((loopInterval / 2) / calFactor))
    if ((loopCurrentTime - loopPreviousTime) > ((loopInterval >> 1) * calFactor))
    {
      loopPreviousTime = loopCurrentTime;       // remember this time
      if (pulseState == LOW) pulseState = HIGH; else pulseState = LOW;
      digitalWrite(13, pulseState);             // to blink onboard LED
      digitalWrite(speedometerPin, pulseState); // to signal Jeep PCM
    }
  }

  wdt_reset();  // kick the dog
}
