//-------------------------------------------------
//-------------------------------------------------
//                 MODO ENSAYO
//-------------------------------------------------
//-------------------------------------------------
#if MODO_ENSAYO == 1
 void pre_ensayo()
 {
   Serial.println("//-------------------------------------------------");
   Serial.println("          CONFIGURACIONES PREVIAS AL ENSAYO");
   Serial.println("//-------------------------------------------------");
   Serial.println("");
    
   inicio_sensores_y_modulos();
  
   Serial.println("Inicio de sensores correcto");
   Serial.println("");
   Serial.println("-------------------------------------------------");
   Serial.println("");

  
 }

 void ensayo()
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
