//-------------------------------------------------
//                   PERFORMANCE
//-------------------------------------------------

void iteration()
{
  performance(); 

  if (performance_status)
  {
    // To know the frequency
    #if FREQD == 1 
      ++cell_freq; 
      if (time_rn - last_cell_freq > 1000)
      {
        Serial.println();
        Serial.print("Cell freq : ");
        Serial.println(cell_freq);
        cell_freq = 0;
        last_cell_freq = time_rn;
      }
    #endif    
  }   
}

void performance() 
{
  sec = (millis() - last_reset_millis) / 1000.;

  // Measures sensors
  data_measure();

  // Request data from BREDA
  requestBreda();

  // Writes in SD
  file_data_update();
}

void performance_started()
{
  radio_timer = 20;
  file_open();

  transducer_set_high_speed(true);
  performance_status = true;
}

void performance_finished()
{
  radio_timer = 1000;

  #if SD_READER == 1
    if (!file_closed) 
      file_close();
  #endif

  power_relay(false);
  transducer_set_high_speed(false);
  
  performance_status = false;
  ignition_started = false;

  last_reset_millis = millis();
  last_reset_micros = last_reset_millis / 1000.;
}

//-------------------------------------------------
//                    IGNITION
//-------------------------------------------------

void power_relay(bool status)
{
  digitalWrite(PIN_RELAY, status);
  digitalWrite(PIN_ALARM, status);
} 

void ignition_toggled(bool status)
{
  last_ignition = millis();
  ignition_started = status;
  power_relay(status);
}

//-------------------------------------------------
//                    WARNINGS
//-------------------------------------------------

void error_warning() 
{
  for (int i=0; i<10; i++)
  {
    digitalWrite(PIN_LED, false);
    delay(200);
    digitalWrite(PIN_LED, true);
    delay(200);
  }
}