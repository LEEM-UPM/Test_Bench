void data_deliver()
{
  #if RADIO == 1
    XBEE_COM.write(wholePack, wholePackSize);
    XBEE_COM.flush();
  #else
    Serial.write(wholePack, wholePackSize);
    Serial.flush();
  #endif  
}

void serial_read()
{
  // Detects anything in Serial
  #if RADIO == 1
    if (XBEE_COM.available()) 
    {
      length = XBEE_COM.readBytesUntil('\n', serialBuffer, serialSize);
      receiving = true;   
    }
  #else
    if (Serial.available()) 
    {
      length = Serial.readBytesUntil('\n', serialBuffer, serialSize);
      receiving = true;   
    }
  #endif

  // Process serial
  if (receiving)
  {
    // Turns off comm LED during LED_timer milis
    digitalWrite(PIN_LED, false);
    last_LED = millis();
    LED_started = false;

    // Orden detection
    if ((length == 2) && (serialBuffer[0] == 251))
    {
      order = serialBuffer[1];
      // Resends order to confirm deliver
      send_order(order);
      // And acts
      obey_order(order);
    } 
    order = 0;
    receiving = false;
  }
}

void send_order(uint8_t order) 
{ 
  #if RADIO == 1
    XBEE_COM.write(serialID, 2);
    XBEE_COM.write(0);
    XBEE_COM.write(order + 3);
    XBEE_COM.write(2);
    XBEE_COM.write(1);
    XBEE_COM.write(order);
    XBEE_COM.flush();
  #else
    Serial.write(serialID, 2);
    Serial.write(0);
    Serial.write(order + 3);
    Serial.write(2);
    Serial.write(1);
    Serial.write(order);
    Serial.flush();
  #endif  
}

void obey_order(uint8_t order)
{
  switch (order) 
  {
    // Conexion established
    case 1:  
    break;

    // Testing
    case 2:  
      testMode = true;
    break;

    // Performing
    case 3:  
      testMode = false;
    break;

    // Reboot
    case 4:  
      performance_finished();  
      send_order(5);
    break;

    // Tare
    case 6:  
      #if W_CELL == 1
        cell.tare(255);
      #endif  
      #if TRANSDUCER == 1
        transducer_set_offset();
      #endif       
      tared = true;
      send_order(7);
    break;
    
    // Start performing
    case 8:  
      performance_started = true;
      if (file_open()) error_warning();
      wholePackSize = miniPackSize * packSize;
      delay(1000);
    break;

    // Stop performing
    case 9:  
      performance_finished();
    break;

    // Ignition
    case 11:  
      #if RELAY == 1
        power_relay(true);
        ignition_started = true;
        last_ignition = millis();
      #endif
    break;
  }
}