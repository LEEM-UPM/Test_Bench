//-------------------------------------------------
//-------------------------------------------------
//                   BENCH CODE
//-------------------------------------------------
//-------------------------------------------------

#include "Header.h"

void setup() 
{
  // LED Control
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, true);

  // Communications
  Wire.begin();
  Serial.begin(230400);

  // Radio
  XBEE_COM.begin(230400);

  // Relay
  pinMode(PIN_RELAY, OUTPUT);

  // Sensor start
  sensor_init();

  // Cell pin
  pinMode(HX_DOUT, INPUT);
}

void loop() 
{
  // Reads an instruction if it arrived
  serial_read();

  // Iteration
  iteration();

  // Reads the transducer (10 kHz)
  #if TRANSDUCER == 1 
    if ((micros() - last_transducer) > transducer_timer)
    {
      transducer_measure();
      last_transducer = micros();
    }
  #endif  
  
  // Turns on the led after one second (Normally because an orden has been sent)
  if (!LED_started && (millis() - last_LED > LED_timer)) 
  {
    LED_started = true;
    digitalWrite(PIN_LED, true);
  }
  
  // Turns off relay after 5 secs
  #if RELAY == 1 
    if (ignition_started && (millis() - last_ignition > ignition_timer)) 
    {
      ignition_started = false;
      power_relay(false);
    }
  #endif  
}