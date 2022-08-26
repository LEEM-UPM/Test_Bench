//-------------------------------------------------
//-------------------------------------------------
//                 MODO TESTEO
//-------------------------------------------------
//-------------------------------------------------
#if MODO_TESTEO == 1
//-------------------------------------------------
//            Calibración célula de carga
//-------------------------------------------------
 #if CALIBRADO_CELULA == 1
   void calibrado_celula()
   {
    Serial.println("//-------------------------------------------------");
    Serial.println("                    Calibrado célula de carga");
    Serial.println("//-------------------------------------------------");
    Serial.println("");
    Serial.println("Calibrando...");
    
    lect = 0;
    for (int i=0; i<50 ; i++)
    {
    lect += celula.read(); 
    }    
    Serial.println("");
    Serial.print("CRO del calibrado = ");
    Serial.println(lect/50); // NUESTRO CRO ES EN 8830
    Serial.println("");
    Serial.println("Calibrado de la célula de carga completado");
    Serial.println("");
    Serial.println("---------------------------------------------------");
    Serial.println("");
   }
  #endif


//-------------------------------------------------
//            Testeo LED de control
//-------------------------------------------------

#if TESTEO_LED == 1
 void testeo_led()
 {
    Serial.println("//-------------------------------------------------");
    Serial.println("           Testeo LED de control");
    Serial.println("//-------------------------------------------------");
    Serial.println("");
  for (uint8_t k = 0; k < 5; k++) 
  {  
    digitalWrite(LED_ERROR, HIGH);
    delay(500);                       
    digitalWrite(LED_ERROR, LOW);   
    delay(500);  
  }
    Serial.println("Fin del test");
    Serial.println("");
    Serial.println("---------------------------------------------------");
    Serial.println("");
 }
#endif


//-------------------------------------------------
//            Prueba funcionamiento Relé
//-------------------------------------------------
#if RELE == 1
  #if TESTEO_RELE == 1
   void testeo_rele()
   {
    Serial.println("//-------------------------------------------------");
    Serial.println("                    Testeo Relé");
    Serial.println("//-------------------------------------------------");
    Serial.println("");
    
     for (uint8_t j = 0; j < 2; j++) 
    {
      rele_cerrado();
      delay(500);                       
      rele_abierto();   
      delay(500);  
    }
    Serial.println("Testeo del relé finalizado");
    Serial.println("");
    Serial.println("---------------------------------------------------");
    Serial.println("");
   }
 #endif
#endif
 
//-------------------------------------------------
//                 Escáner I2C
//-------------------------------------------------
#if ESCANER_I2C == 1
  void escaner_i2c()
  {
    byte error, address;
    int nDevices;
    Serial.println("//-------------------------------------------------");
    Serial.println("                    Escáner I2C");
    Serial.println("//-------------------------------------------------");
    Serial.println("");
    Serial.println("Escaneando...");
   
    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
   
      if (error == 0)
      {
        Serial.print("Dispositivo I2C en 0x");
        if (address<16)
          Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");
   
        nDevices++;
      }
      else if (error==4)
      {
        Serial.print("Error desconocido en la dirección 0x");
        if (address<16)
          Serial.print("0");
        Serial.println(address,HEX);
      }    
    }
    if (nDevices == 0)
      Serial.println("No se han encontrado dispositivos I2C");
    else
      Serial.println("Escáner I2C completado");
      Serial.println("");
      Serial.println("---------------------------------------------------");
      Serial.println("");
    delay(3000);
  }
#endif


//-------------------------------------------------
//               Toma de datos
//-------------------------------------------------

#if TESTEO_DE_DATOS == 1
void inicio_de_datos()
{
 Serial.println("//-------------------------------------------------");
 Serial.println("                Test toma de datos");
 Serial.println("//-------------------------------------------------");
 Serial.println("");
  
 inicio_sensores_y_modulos();

 Serial.println("Inicio de sensores correcto");
 Serial.println("");
 Serial.println("-------------------------------------------------");
 Serial.println("");

}
  
void testeo_de_datos()
{
  toma_de_datos();
  visualizacion_datos_serie();
 #if LECTOR_SD == 1
  escritura_sd();
 #endif
 #if EEPROM_I2C == 1
    EEPROM_I2C_Almacena_datos();
 #endif 
}
#endif

#endif
