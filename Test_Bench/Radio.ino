void data_deliver()
{
  RADIO_OUT.write(wholePack, wholePackSize);
  RADIO_OUT.flush();
}

void serial_read()
{
  // Detects anything in Serial
  if (RADIO_OUT.available()) 
    length = RADIO_OUT.readBytesUntil('\n', serialBuffer, serialSize);
  else
    return;

  /*
  Serial.println();
  for (int i = 0; i < serialSize; ++i) 
  {
    Serial.print(serialBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();
  */

  // Orden detection
  order = order_checking(serialBuffer, serialSize);
  if (order == -1) return;

  // Turns off comm LED during LED_timer milis
  digitalWrite(PIN_LED, false);
  last_LED = millis();
  LED_started = false;

  // Resends order to confirm deliver
  send_order(order);
  // Acts
  obey_order(order);
  // Sends it to BREDA
  send_BREDA_order(order); 

  order = 0;
}

void send_order(uint8_t order) 
{ 
  RADIO_OUT.write(serialID, 2);
  RADIO_OUT.write(order + 3);
  RADIO_OUT.write(0);
  RADIO_OUT.write(2);
  RADIO_OUT.write(1);
  RADIO_OUT.write(order);
  RADIO_OUT.write('\n');
  RADIO_OUT.flush();
}

void obey_order(uint8_t order)
{
  switch (order) 
  {
    // Conexion established
    case arduinoConnected:  
      transducer_enabled = true;
    break;

    // Transducer activated
    case enableTransducer:  
      transducer_enabled = true;
    break;

    // Transducer deactivated
    case disableTransducer: 
      transducer_enabled = false;
    break;

    // Reboot
    case reboot:  
      performance_finished();  
      send_order(rebootComplete);
    break;

    // Tare
    case tare:  
      #if W_CELL == 1
        cell.tare(255);
      #endif  
      #if TRANSDUCER == 1
        transducer_set_offset();
      #endif       
      send_order(tareComplete);
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
      #if RELAY == 1
        ignition_started = true;
        //ignition_started = !ignition_started;
        power_relay(ignition_started);
        last_ignition = millis();
      #endif
    break;

    // Stop ignition (Centralita)
    case stopIgnition:  
      #if RELAY == 1
        power_relay(false);
        ignition_started = false;
      #endif
    break;
  }
}