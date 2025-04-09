//-------------------------------------------------
//-------------------------------------------------
//                   BENCH CODE
//-------------------------------------------------
//-------------------------------------------------

#include "Config.h"

void setup() 
{
  Serial.begin(460800);

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

  // Sensors 
  bool error = false;

  // DHT 
  #if DHT_22 == 1
    dht.begin();
  #endif

  // Cell 
  #if W_CELL == 1
    cell.begin(HX_DOUT, HX_SCK);
    pinMode(HX_DOUT, INPUT);
  #endif

  // SD 
  #if SD_READER == 1
    if (!SDF.begin(SdioConfig(FIFO_SDIO))) 
    {
      send_order(errorSD);
      error = true;
      SD_ready = false;
    }
  #endif

  // Transducer 
  #if TRANSDUCER == 1
    adc->adc0->setAveraging(8);
    adc->adc0->setResolution(12); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed

    transducer_set_high_speed(false);
  #endif

  if (error) 
    error_warning();

  pack_header();  
}

void loop() 
{
  // Reads an instruction if it arrived
  if (RADIO_OUT.available())
    serial_read();

  // Gather data
  if (digitalRead(HX_DOUT)) 
    iteration();

  // Telemetry
  if ((millis() - last_radio) > radio_timer)
  {
    last_radio = millis();
    // Sets the pack
    pack_change();
    // Sends every 4 minipacks
    data_deliver();
  }

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

  // Transducer offset
  #if TRANSDUCER == 1 
    if ((transducer_offset_activated) && ((millis() - last_transducer_offset) > 1000))
    {
      transducer_offset /= transducer_counter_offset;
      transducer_offset_activated = false;
    }
  #endif 
  
  // Turns on the led after one second (Normally because an orden has been sent)
  if (!LED_started && (millis() - last_LED > LED_timer)) 
  {
    LED_started = true;
    digitalWrite(PIN_LED, true);
  }
  
  // Turns off relay after 5 secs (In case it is on and an hydrostatic test is not going on)
  if ((!hydrostatic_enabled) && (ignition_started) && (millis() - last_ignition > ignition_timer))
  {
    last_ignition = millis();
    obey_order(stopIgnition);
  }  

  // Write it in the SD
  if (RB_data.bytesUsed() >= 512 && !file_data.isBusy())
  {
    RB_data.writeOut(512);
  }

  #if TRANSDUCER == 1 
    if (RB_pressure.bytesUsed() >= 512 && !file_pressure.isBusy())
    {
      RB_pressure.writeOut(512);
    }
  #endif
}