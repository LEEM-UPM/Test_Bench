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
  RADIO_OUT.begin(460800);

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

  // Prints the transducer freq
  #if TRANSDUCER == 1 && FREQD == 1
    if ((transducer_enabled) && ((millis() - last_transducer_freq) > 1000))
    {
      last_transducer_freq = millis();
      Serial.println();
      Serial.print("Transducer freq : ");

      noInterrupts();
      Serial.println(transducer_freq);
      transducer_freq = 0;
      interrupts();
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
    if ((ignition_started) && (millis() - last_ignition > ignition_timer))
    {
      ignition_started = false;
      power_relay(false);
      digitalWrite(PIN_LED, true);
    }  
  #endif  

  #if SD_READER == 1
    // Write it in the round buffer
    if (RB_data.bytesUsed() >= 512 && !file_data.isBusy())
      RB_data.writeOut(512);

    #if TRANSDUCER == 1
      // Write it in the round buffer
      if ((RB_pressure.bytesUsed() >= 512) && !file_pressure.isBusy())
        RB_pressure.writeOut(512);
    #endif
  #endif  
}