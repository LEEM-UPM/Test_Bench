//-------------------------------------------------
//-------------------------------------------------
//            INICIO MÓDULOS Y SENSORES
//-------------------------------------------------
//-------------------------------------------------

void inicio_sensores()
{
  bool error = false;

  rele(false);

  #if DEBUG == 1
    Serial.println("//-------------------------------------------------");
    Serial.println("          CONFIGURACIONES PREVIAS AL ENSAYO");
    Serial.println("//-------------------------------------------------");
    Serial.println("");
  #endif

  // Inicio y verificación BMP280
  #if BMP_280 == 1
    if (!bmp->begin(0x76))
    {
      #if DEBUG == 1
        Serial.println("Error al iniciar el BMP280");
      #endif
      envio_orden(11);
      error = true;
    }
  #endif

  // Inicio del DHT-22
  #if DHT_22 == 1
    dht.begin();
  #endif

  // Inicio y verificación célula de carga
  #if CELULA_CARGA == 1
    celula.begin(HX_DOUT, HX_SCK);
    if (!celula.is_ready())
    {
      #if DEBUG == 1
        Serial.println("Error al iniciar la célula de carga");
      #endif
      envio_orden(12);
      error = true;
    }
  #endif

  // Inicio y verificación del lector SD
  #if LECTOR_SD == 1
    if (!SD.begin(BUILTIN_SDCARD)) 
    {
      #if DEBUG == 1
        Serial.println("Error al iniciar el lector SD");
      #endif
      envio_orden(13);
      error = true;
    }

    error = abrir_archivo();
  #endif

  if (error) error_inicio();
  else 
  {
    #if DEBUG == 1
      Serial.println("Inicio de sensores correcto");
      Serial.println("");
      Serial.println("-------------------------------------------------");
      Serial.println("");
    #endif
  }

  #if TRANSDUCTOR == 1
    set_offset_transductor();
  #endif

  encabezamiento_pack();  
  wholePackSize = miniPackSize;
}

//-------------------------------------------------
//                ABRIR ARCHIVO SD
//-------------------------------------------------

bool abrir_archivo()
{
  #if LECTOR_SD == 1
    int i = 0; 
    String newName = i + data_name;

    while (SD.exists(newName.c_str())) newName = ++i + data_name;

    archivo_datos = SD.open(newName.c_str(), FILE_WRITE);
    #if TRANSDUCTOR == 1
      newName = i + transductor_name;
      archivo_presion = SD.open(newName.c_str(), FILE_WRITE);
    #endif

    if (archivo_datos == NULL) 
    {
      #if DEBUG == 1
        Serial.println("Error Archivo SD");
      #endif
      envio_orden(14);
      error_inicio();
      archivoIsClosed = false;
      SDReady = false;
      return true;
    }

    archivo_datos.println("Time, Thrust, Chamber_Pressure, BMP_Temperatura, BMP_Pressure, DHT_Humidity");
    #if TRANSDUCTOR == 1
      archivo_datos.println("Time, Chamber_Pressure");
    #endif
  #endif  

  return false;
}

//-------------------------------------------------
//-------------------------------------------------
//                  FUNCIONES
//-------------------------------------------------
//-------------------------------------------------

//-------------------------------------------------
//                ITERACION ENSAYO
//-------------------------------------------------

void iteracion()
{
  if (digitalRead(HX_DOUT)) return;

  tiempo = millis();

  if (ensayoStarted)
  {
    // Ensayo
    ensayo(); 
    lastTime = tiempo;     

    // Para saber la frecuencia
    #if FREQD == 1 
      ++freq; 
      if (tiempo - lastFreq > 1000)
      {
        Serial.println();
        Serial.println(freq);
        freq = 0;
        lastFreq = tiempo;
      }
    #endif    
  }   
  // Cada segundo sin ensayar se ve una muestra de datos
  else if (tiempo - lastTime >= 1000) 
  {
    lastTime = tiempo;
    ensayo();
  } 
}

void ensayo() 
{
  // Toma los datos de los sensores
  toma_datos();
  
  // Cambia lo necesario del pack
  cambio_pack();

  // Debugea los datos si esta activado
  #if DEBUG == 1
    serial_datos();
  #endif  

  // Enviar cada x mini paquetes
  for (int i = 0; i < miniPackSize; ++i) wholePack[i + packPos] = miniPack[i];
  packPos += miniPackSize;

  if (packPos == wholePackSize)
  {
    packPos = 0;
    #if DEBUG == 0
      envio_datos();         
    #endif
  }  

  // Escribir SD
  #if LECTOR_SD == 1
    if (ensayoStarted && SDReady) escritura_datos_SD();
  #endif
}

//-------------------------------------------------
//                     RELE
//-------------------------------------------------

void rele(bool estado)
{
  digitalWrite(PIN_RELE, !estado);
} 

//-------------------------------------------------
//                     AVISOS
//-------------------------------------------------

void error_inicio() 
{
  for (int i=0; i<10; i++)
  {
    digitalWrite(PIN_LED, false);
    delay(100);
    digitalWrite(PIN_LED, true);
    delay(100);
  }
}

//-------------------------------------------------
//                   TRANSDUCTOR
//-------------------------------------------------

void set_offset_transductor()
{
  // Leemos y hacemos media
  long lectura_transductor = 0;

  for (int i = 0; i < 500; ++i)
  {
    lectura_transductor += analogRead(PIN_TRANSDUCTOR);
    delay(1);
  }

  offset_transductor = lectura_transductor / (max_v_transductor * 500) * 250;

}

void funciones_transductor()
{
  // Leemos y corregimos el valor a bares
  lectura_transductor = analogRead(PIN_TRANSDUCTOR);

  presion_transductor = (int) ((lectura_transductor / max_v_transductor) * 250 - offset_transductor);

  avg_transductor += presion_transductor;
  ++counter_transductor;

  // Escribimos en la SD
  //if (ensayoStarted) escritura_presion_SD();
}

//-------------------------------------------------
//-------------------------------------------------
//              TRATAMIENTO DE DATOS
//-------------------------------------------------
//-------------------------------------------------

//-------------------------------------------------
//                 TOMA DE DATOS
//-------------------------------------------------

void toma_datos()
{
  // Tiempo 
  sec = (millis() - lastReset) / 1000.;

  // Datos del BMP_280
  #if BMP_280 == 1
    temperatura_BMP = bmp->readTemperature();
    presion_BMP     = bmp->readPressure();
  #endif

  // DHT22
  #if DHT_22 == 1
    if (millis() - DHT22Since > 2000) 
    {
      humedad_DHT22     = dht.readHumidity();
      temperatura_DHT22 = dht.readTemperature();
      tiempo_DHT22      = millis();
    }
  #endif

  // Célula de carga
  #if CELULA_CARGA == 1
    empuje = mult_celula * abs((celula.get_value(1)));
  #endif
}

//-------------------------------------------------
//              CREACION DE PAQUETE
//-------------------------------------------------

union byteConverter
{
	float floatP;
  int intP;
	byte byteP[4];
};

void encabezamiento_pack()
{
  // Añade el encabezamiento de cada minipaquete, 2 bytes de ID y 24 bytes de longitud extra
  miniPack[0] = serialID[0];
  miniPack[1] = serialID[1];
  miniPack[4] = 0X18;
}

void cambio_pack()
{
  // Tiempo
  float_to_byte(sec, 5);
      
  // Datos de la célula de carga
  float_to_byte(empuje, 9);

  // Datos del Transductor
  float true_pressure = (avg_transductor / counter_transductor) * max_p_transductor;
  float_to_byte(true_pressure, 13);
  avg_transductor = 0;
  counter_transductor = 0;

  // Datos del BMP280
  float_to_byte(temperatura_BMP, 17);
  float_to_byte(presion_BMP, 21);

  // Datos del DHT22
  float_to_byte(humedad_DHT22, 25);

  // CheckSum
  int check = 0;
  byteConverter toInt;

  for (int i = 4; i < miniPackSize; ++i) check += miniPack[i];
  toInt.intP = check;
  for (int i = 0; i < 2; ++i) miniPack[2 + i] = toInt.byteP[i];
}

void float_to_byte(float var, int pos)
{
  byteConverter conv;
  conv.floatP = var;

  for (int i = 0; i < 4; ++i) miniPack[pos + i] = conv.byteP[i];
}

//-------------------------------------------------
//              ENVIO DE LOS DATOS
//-------------------------------------------------

void envio_datos()
{
  #if TELEMETRIA == 1
    XBEE_COM.write(wholePack, wholePackSize);
    XBEE_COM.flush();
  #else
    Serial.write(wholePack, wholePackSize);
    Serial.flush();
  #endif  
}

//-------------------------------------------------
//           VISUALIZACION DE LOS DATOS
//-------------------------------------------------

void serial_datos()
{
  #if TELEMETRIA == 1
    // Inicio de Cadena
    XBEE_COM.println("Datos :");

    // Tiempo
    XBEE_COM.println(sec);  
        
    // Datos de la célula de carga
    XBEE_COM.println(empuje);

    // Datos del BMP280
    XBEE_COM.println(temperatura_BMP);
    XBEE_COM.println(presion_BMP);

    // Datos del DHT22
    XBEE_COM.println(humedad_DHT22);

  #else
    // Inicio de Cadena
    Serial.println("Datos :");

    // Tiempo
    Serial.println(sec);  
        
    // Datos de la célula de carga
    Serial.println(empuje);

    // Datos del BMP280
    Serial.println(temperatura_BMP);
    Serial.println(presion_BMP);

    // Datos del DHT22
    Serial.println(humedad_DHT22);
  #endif  
}

//-------------------------------------------------
//                ESCRITURA EN SD
//-------------------------------------------------

void escritura_datos_SD ()
{
  archivo_datos.write(miniPack, miniPackSize);
  archivo_datos.write("\n");
  archivo_datos.flush();
}

void escritura_presion_SD()
{
  int tiempo_transductor = (micros() / 1000. - lastReset) / 1000.;
  archivo_presion.print(tiempo_transductor, 5);
  archivo_presion.print(", ");
  archivo_presion.print(presion_transductor * max_p_transductor);
  archivo_presion.print(", ");
  archivo_presion.println(lectura_transductor);
  archivo_datos.flush();
}