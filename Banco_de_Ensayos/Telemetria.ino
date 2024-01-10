//-------------------------------------------------
//-------------------------------------------------
//                 COMUNICACIONES
//-------------------------------------------------
//-------------------------------------------------

//-------------------------------------------------
//               LECTURA DEL SERIAL
//-------------------------------------------------

void leer_serial()
{
  // Si detecta algo en el Serial lo recoge
  #if TELEMETRIA == 1
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

  // Procesa los datos
  if (receiving)
  {
    // Enciende led de comunicaciones
    digitalWrite(PIN_LED, false);
    ledSince = millis();
    ledStarted = false;

    // Detecta si es una orden
    if ((length == 2) && (serialBuffer[0] == (char) -5))
    {
      order = serialBuffer[1];
      // Reenvia la orden para confirmar que la ha recibido
      envio_orden(order);
      // Y actua en consecuencia
      rec_orden(order);
    } 
    order = 0;
    receiving = false;
  }
}

//-------------------------------------------------
//               ENVIO DE LA ORDEN
//-------------------------------------------------

void envio_orden(char order) 
{ 
  #if TELEMETRIA == 1
    XBEE_COM.write(serialID, 2);
    XBEE_COM.write(0);
    XBEE_COM.write(order + 1);
    XBEE_COM.write(1);
    XBEE_COM.write(order);
    XBEE_COM.flush();
  #else
    Serial.write(serialID, 2);
    Serial.write(0);
    Serial.write(order + 1);
    Serial.write(1);
    Serial.write(order);
    Serial.flush();
  #endif  
}

//-------------------------------------------------
//           INTERPRETACION DE LA ORDEN
//-------------------------------------------------

void rec_orden(char order)
{
  switch (order) 
  {
    // Arduino conectado
    case 1:  
    break;

    // Testeo
    case 2:  
      testMode = true;
    break;

    // Ensayo
    case 3:  
      testMode = false;
    break;

    // Reset
    case 4:  
      #if LECTOR_SD == 1
        if (archivoIsClosed)
        {
          archivo_datos.close();
          #if TRANSDUCTOR == 1
            archivo_presion.close();
          #endif
          abrir_archivo();
        }
      #endif

      rele(false);
      ensayoStarted = false;
      ignitionStarted = false;
      tared = false;
      wholePackSize = miniPackSize;
      lastReset = millis();
    break;

    // Tarar
    case 5:  
      celula.tare(255);
      set_offset_transductor();
      tared = true;
      envio_orden(6);
    break;
    
    // Inicia ensayo
    case 7:  
      ensayoStarted = true;
      wholePackSize = miniPackSize * packSize;
      delay(1000);
    break;

    // Finaliza ensayo
    case 8:  
      #if LECTOR_SD == 1
        archivo_datos.close();   
        #if TRANSDUCTOR == 1
          archivo_presion.close();
        #endif 
        archivoIsClosed = true;
      #endif
      ensayoStarted = false;
      wholePackSize = miniPackSize;
      lastReset = millis();
      delay(1000);
    break;

    // Ignición
    case 9:  
      #if RELE == 1
        rele(true);
        ignitionStarted = true;
        ignitionSince = millis();
      #endif
    break;
  }
}