//-------------------------------------------------
//-------------------------------------------------
//            INICIO MÓDULOS Y SENSORES
//-------------------------------------------------
//-------------------------------------------------

void inicio_sensores_y_modulos()
{
// Inicio y verificación BMP280
#if PRESION_BMP280== 1
  if (!bmp->begin(0x76))
  {
   #if DEBUG == 1
    Serial.print("Error al iniciar el BMP280");
   #endif
   error_inicio();
  }
#endif

// Inicio y verificación DHT11
#if HUMEDAD_DHT11 == 1
  dht->begin();
#endif

// Inicio y verificación DS18B20
#if TEMPERATURA_DALLAS == 1
 sensorDS18B20.begin();
#endif

// Inicio y verificación del lector SD
#if LECTOR_SD == 1
  if (!SD.begin(SSpin)) 
  {
   #if DEBUG == 1
    Serial.println("Error al iniciar el lector SD");
   #endif
   error_inicio(); 
  }
  archivo = &(SD.open("datos.txt", FILE_WRITE));
  if (archivo == NULL) 
   {
   #if DEBUG == 1
    Serial.println("Error Archivo SD");
   #endif
   error_inicio();
   }
#endif

// Inicio y verificación EEPROM I2C
#if EEPROM_I2C  == 1
  if (!init_EEPROMI2C() == NULL)
  {
   #if DEBUG == 1
    Serial.print("Error al iniciar la EEPROM");
   #endif
   error_inicio();
  }
#endif
}

//-------------------------------------------------
//                 TOMA DE DATOS
//-------------------------------------------------

void toma_de_datos()
{
// Tiempo
 tiempo = millis();
 
// Datos del BMP280
#if PRESION_BMP280 == 1
  temperatura_BMP = bmp->readTemperature();
  presion_BMP     = bmp->readPressure();
  altura_BMP      = bmp->readAltitude(presion_referencia); 
#endif

// Datos del DHT22
#if HUMEDAD_DHT11 == 1
  if (millis() - tiempo_DHT11 > 2000) 
  {
    humedad_DHT11     = dht->readHumidity();
    temperatura_DHT11 = dht->readTemperature();
    tiempo_DHT11      = millis();
  }
#endif  

// Célula de carga
#if CELULA_CARGA == 1
  lect = celula.read();
  mes = (lect-5950)*3179.35/11875;
#endif

// Datos del Acelerómetro
#if ACELEROMETRO == 1
  WT_I2C.readI2C();
#endif

// Datos Temperatura Dallas
#if TEMPERATURA_DALLAS == 1
  sensorDS18B20.requestTemperatures();
 #if DALL_1 == 1
  dallas_1 = sensorDS18B20.getTempCByIndex(0);
 #endif
 #if DALL_2 == 1
  dallas_2 = sensorDS18B20.getTempCByIndex(1);
 #endif
 #if DALL_3 == 1
  dallas_3 = sensorDS18B20.getTempCByIndex(2);
 #endif
#endif

// Datos Termopares
#if TERMOPARES == 1
 #if TERM_1 == 1
  termopar_1 = par1.readCelsius ();
 #endif
 #if TERM_2 == 1
  termopar_2 = par2.readCelsius ();
 #endif
 #if TERM_3 == 1
  termopar_3 = par3.readCelsius ();
 #endif
 #if TERM_4 == 1
  termopar_4 = par4.readCelsius ();
 #endif
#endif
}


//-------------------------------------------------
//           VISUALIZACION DE LOS DATOS
//-------------------------------------------------

void visualizacion_datos_serie()
{
  
#if DEBUG == 1
// Tiempo
    Serial.print(tiempo);
    Serial.print(", ");
    
// Datos del BMP280
  #if PRESION_BMP280 == 1
    Serial.print(temperatura_BMP);
    Serial.print(", ");
    Serial.print(presion_BMP);
    Serial.print(", ");
 //   Serial.print(altura_BMP);
  //  Serial.print(", ");
  #endif

// Datos del DHT11
 #if HUMEDAD_DHT11 == 1
    Serial.print(humedad_DHT11);
    Serial.print(", ");
    Serial.print(temperatura_DHT11);
    Serial.print(", ");
 #endif

// Datos de la célula de carga
#if CELULA_CARGA == 1
    Serial.print(lect);
    Serial.print(", ");
    Serial.print(mes);
    Serial.print(", ");
#endif

// Datos del acelerómetro
 #if ACELEROMETRO == 1
    Serial.print(", ");
    Serial.print(WT_I2C.acc[0]);
    Serial.print(", ");
    Serial.print(WT_I2C.acc[1]);
    Serial.print(", ");
    Serial.print(WT_I2C.acc[2]);
    Serial.print(", ");
 #endif

// Temperatura DS18B20
#if TEMPERATURA_DALLAS == 1
 sensorDS18B20.requestTemperatures();
 #if DALL_1 == 1
   Serial.print(dallas_1);
   Serial.print(", ");
 #endif
 #if DALL_2 == 1
   Serial.print(dallas_2);
   Serial.print(", ");
 #endif
 #if DALL_3 == 1
   Serial.print(dallas_3);
   Serial.print(", ");
 #endif
#endif

// Termopares
#if TERMOPARES == 1
   #if TERM_1 == 1
    Serial.print(termopar_1);
    Serial.print(", ");
   #endif
   #if TERM_2 == 1
    Serial.print(termopar_2);
    Serial.print(", ");
   #endif
   #if TERM_3 == 1
    Serial.print(termopar_3);
    Serial.print(", ");
   #endif
   #if TERM_4 == 1
    Serial.print(termopar_4);
    Serial.print(", ");
   #endif
#endif

  Serial.println("");
#endif
}


//-------------------------------------------------
//                ESCRITURA EN SD
//-------------------------------------------------

void escritura_sd()
{
#if LECTOR_SD == 1
// Caractéres de control
  archivo->write("EE");
  archivo->write((byte*)&tiempo,4); 

// Presion_BMP280
 #if PRESION_BMP280 == 1
  archivo->write((byte*)&temperatura_BMP,4);  
  archivo->write((byte*)&presion_BMP,4); 
 // archivo->write((byte*)&altura_BMP,4);
 #endif

// Humedad DHT11
 #if HUMEDAD_DHT11 == 1
  archivo->write((byte*)&humedad_DHT11,4);
  archivo->write((byte*)&temperatura_DHT11,4);
 #endif

// Célula de carga
 #if CELULA_CARGA == 1
  archivo->write((byte*)&lect,4);
  archivo->write((byte*)&mes,4);
 #endif

// Acelerómetro
 #if ACELEROMETRO == 1
  archivo->write((byte*)&WT_I2C.acc[0],4); 
  archivo->write((byte*)&WT_I2C.acc[1],4); 
  archivo->write((byte*)&WT_I2C.acc[2],4);
 #endif

// Temperatura DS18B20
 #if TEMPERATURA_DALLAS == 1
  #if DALL_1 == 1
   archivo->write((byte*)&dallas_1,4); 
  #endif
  #if DALL_2 == 1
   archivo->write((byte*)&dallas_2,4);
  #endif
  #if DALL_3 == 1 
   archivo->write((byte*)&dallas_3,4);
  #endif
 #endif

// Termopares
 #if TERMOPARES == 1
  #if TERM_1 == 1
   archivo->write((byte*)&termopar_1,4);
  #endif
  #if TERM_2 == 1
   archivo->write((byte*)&termopar_2,4);
  #endif
  #if TERM_3 == 1
   archivo->write((byte*)&termopar_3,4);
  #endif
  #if TERM_4 == 1
   archivo->write((byte*)&termopar_4,4);
  #endif
 #endif


 archivo->flush();
#endif
}
