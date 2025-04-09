//-------------------------------------------------
//                  MEASUREMENTS
//-------------------------------------------------

void data_measure()
{
  // DHT
  #if DHT_22 == 1
    if (millis() - last_DHT > DHT_timer) 
    {
      DHT_hum = dht.readHumidity();
      DHT_temp = dht.readTemperature();
      last_DHT = millis();
    }
  #endif

  // Cell
  #if W_CELL == 1
    cell_thrust = cell_f * cell.get_value(1);
  #endif
}

//-------------------------------------------------
//                    TRANSDUCER
//-------------------------------------------------

void transducer_set_high_speed(bool enable)
{
  #if TRANSDUCER == 1
    adc->adc0->stopTimer();
    adc->adc0->startSingleRead(PIN_TRANSDUCER); // call this to setup everything before the Timer starts, differential is also possible
    adc->adc0->enableInterrupts(transducer_measure);

    if (!enable)
    {
      adc->adc0->setAveraging(100); // set number of averages
      adc->adc0->startTimer(10000); //frequency in Hz
    }
    else
    {
      adc->adc0->setAveraging(5); // set number of averages
      adc->adc0->startTimer(1000000 / transducer_timer); //frequency in Hz
    }
  #endif
}

void transducer_set_offset()
{
  #if TRANSDUCER == 1
    // Read and get the average
    transducer_offset = 0;
    transducer_counter_offset = 0;
    last_transducer_offset = millis();
    transducer_offset_activated = true;
  #endif  
}

void transducer_measure()
{
  #if TRANSDUCER == 1
    // Read analog pin and change it to bars
    transducer_raw = adc->adc0->readSingle();

    if (transducer_offset_activated)
    {
      transducer_offset += transducer_raw;
      ++transducer_counter_offset;
    }

    transducer_pressure = (transducer_raw - transducer_offset) * transducer_const;
    //transducer_avg += transducer_pressure;

    transducer_avg += transducer_raw;
    ++transducer_counter;
    ++transducer_freq;

    // Write in SD
    if (performance_status) file_pressure_update();
  #endif
}