//-------------------------------------------------
//-------------------------------------------------
//                   BENCH CODE
//-------------------------------------------------
//-------------------------------------------------

#include "Config.h"

void setup() 
{
  // LED Control
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, true);

  // Communications
  Wire.begin();

  // Radio
  RADIO_OUT.begin(230400);

  // Relay
  pinMode(PIN_RELAY, OUTPUT);

  // Alarm
  pinMode(PIN_ALARM, OUTPUT);

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

  #if TRANSDUCER == 1 
    // Reads the transducer (10 kHz)
    if ((transducer_enabled) && ((micros() - last_transducer) > 100))
    {
      transducer_measure();
      ++transducer_freq;
      last_transducer = micros();
    }

    // Prints the transducer freq
    #if FREQD == 1
      if ((transducer_enabled) && ((millis() - last_transducer_freq) > 1000))
      {
        Serial.println();
        Serial.print("Transducer freq : ");
        Serial.println(transducer_freq);

        last_transducer_freq = millis();
        transducer_freq = 0;
      }
    #endif
  #endif  
  
  // Turns on the led after one second (Normally because an orden has been sent)
  if (!LED_started && (millis() - last_LED > LED_timer)) 
  {
    LED_started = true;
    digitalWrite(PIN_LED, true);
  }
  
  // Turns off relay after 5 secs
  #if RELAY == 1 
    if ((ignition_started) && (millis() - last_ignition > ignition_timer))
    {
      ignition_started = false;
      power_relay(false);
      digitalWrite(PIN_LED, true);
    }  
  #endif  

}