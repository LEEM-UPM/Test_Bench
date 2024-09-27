//-------------------------------------------------
//                   PERFORMANCE
//-------------------------------------------------

void iteration()
{
  if (performance_status)
  {
    // Performance
    performance(); 

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
  // If not performing, sends data one time per second 
  else if ((millis() - last_time) >= 1000) 
  {
    last_time = millis();
    performance();
  } 
}

void performance() 
{
  sec = (millis() - last_reset_millis) / 1000.;

  // Measures sensors
  data_measure();

   // Request data from BREDA
  requestBreda();

  // Sets the pack
  pack_change();

  // Sends every 4 minipacks
  for (int i = 0; i < miniPackSize; ++i) 
    wholePack[i + packPos] = miniPack[i];
  packPos += miniPackSize;

  if (packPos == wholePackSize)
  {
    packPos = 0;
    data_deliver();  
  }  

  // Writes in SD
  file_data_update();
}

void performance_started()
{
  wholePackSize = miniPackSize * packSize;
  file_open();

  transducer_set_high_speed(true);
  performance_status = true;
}

void performance_finished()
{
  #if SD_READER == 1
    if (!file_closed) 
      file_close();
  #endif

  power_relay(false);
  transducer_set_high_speed(false);
  
  performance_status = false;
  ignition_started = false;
  wholePackSize = miniPackSize;
  packPos = 0;

  last_reset_millis = millis();
  last_reset_micros = last_reset_millis / 1000.;
}

//-------------------------------------------------
//                    IGNITION
//-------------------------------------------------

void power_relay(bool status)
{
  digitalWrite(PIN_RELAY, !status);
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