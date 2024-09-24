void data_deliver()
{
  RADIO_OUT.write(wholePack, wholePackSize);
  RADIO_OUT.flush();
}

void serial_read()
{ 
  length = RADIO_OUT.readBytesUntil('\n', serialBuffer, serialSize);

  // Orden detection
  order = order_checking(serialBuffer, length);
  Serial.println(order);
  if (order == 255) 
    return;

  // Turns off comm LED during LED_timer milis
  digitalWrite(PIN_LED, false);
  last_LED = millis();
  LED_started = false;

  // In case is sending turn off order from interface
  if ((order == startIgnition) && ignition_started) 
    order = stopIgnition;

  // Acts
  obey_order(order);
}

void send_order(uint8_t order) 
{ 
  RADIO_OUT.write(serialID, 2);
  RADIO_OUT.write(HERMES_ID);
  RADIO_OUT.write(order + 3);
  RADIO_OUT.write(2);
  RADIO_OUT.write(1);
  RADIO_OUT.write(order);
  RADIO_OUT.write('\n');
  RADIO_OUT.flush();
}

void obey_order(uint8_t order)
{
  // Resends order to confirm deliver
  send_order(order);

  // Sends it to BREDA
  send_BREDA_order(order); 

  switch (order) 
  {
  // Conexion established
  case arduinoConnected:  
    transducer_enabled = true;
    hydrostatic_enabled = false;
    break;

  // Transducer activated
  case enableTransducer:  
    transducer_enabled = true;
    break;

    // Transducer deactivated
  case disableTransducer: 
    transducer_enabled = false;
    break;

  // Hydrostatic activated
  case enableHydroStatic:  
    hydrostatic_enabled = true;
    break;

    // Hydrostatic deactivated
  case disableHydroStatic: 
    hydrostatic_enabled = false;
    break;  

  // Tare
  case tare:  
    #if W_CELL == 1
      cell.tare(255);
    #endif  
    #if TRANSDUCER == 1
      transducer_set_offset();
    #endif       
    break;
    
  // Start performing
  case startPerforming:  
    performance_started();
    break;

    // Stop performing
  case stopPerforming:  
    performance_finished();
    break;

  // Start ignition
  case startIgnition:  
    ignition_toggled(true);
    break;

  // Stop ignition 
  case stopIgnition:  
    ignition_toggled(false);     
    break;
  }
}