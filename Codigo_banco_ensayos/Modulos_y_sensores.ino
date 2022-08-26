//-------------------------------------------------
//                 EEPROM I2C
//-------------------------------------------------
/*
#if EEPROM_I2C == 1

boolean init_EEPROMI2C() 
{
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  if (Wire.endTransmission() != 0) 
  {
    return 1;
  }
  return 0;
}

// Almacenar los siguientes datos:
void EEPROM_I2C_Almacena_datos() 
{
  if (eeprom_mem < (65536 - 35)) 
  {
    byte paquete[30];  // No se pueden guardar paquetes superiores a 30 bytes, se llena el buffer I2C
    uint16_t aux = FLIGHT_TIME;
    uint16_to_2byte(aux, &(paquete[0]));
    #if ACELEROMETRO_KX134 == 1
      float_to_4byte(&X_out, &(paquete[2]));
      float_to_4byte(&Y_out, &(paquete[6]));
      float_to_4byte(&Z_out, &(paquete[10]));
    #endif
    #if PRESION_BMP280 == 1
      float_to_4byte(&presion_BMP, &(paquete[14]));
      float_to_4byte(&altura_BMP, &(paquete[18]));
    #endif
    #if GPS == 1
      float_to_4byte(&GPS_LAT, &(paquete[22]));
      float_to_4byte(&GPS_LON, &(paquete[26]));
    #endif
    writeEEPROM_Page(eeprom_mem, paquete, 30);
    eeprom_mem += 30;
  }
}

// Almacenar máximo 30 bytes seguidos
void writeEEPROM_Page(uint16_t address, byte *val, byte tam)
{
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((uint8_t)(address >> 8));   // MSB
  Wire.write((uint8_t)(address & 0xFF)); // LSB
  Wire.write(val, tam);
  Wire.endTransmission();
}

// Guardar el float en &aux, &aux+1, &aux+2, &aux+3
void float_to_4byte(float* var, byte* aux) 
{
  byte* p = (byte*)var;
  for (char i = 3; i >= 0; i--) 
  {
    *(aux + i) = *p;
    p++;
  }
}

// Conversión de los bytes a float
void _4byte_to_float(byte* aux, float *out) 
{
  uint32_t mem_aux = 0;
  mem_aux |= aux[3];
  mem_aux |= (uint32_t)(aux[2]) << 8;
  mem_aux |= (uint32_t)(aux[1]) << 16;
  mem_aux |= (uint32_t)(aux[0]) << 24;
  *(out) = *((float*)&mem_aux);
}

// Guardar el uint16_t MSB, LSB
void uint16_to_2byte(uint16_t dato_in, byte* dir_dato_out) 
{
  *(dir_dato_out) = (byte)(dato_in >> 8);
  *(dir_dato_out + 1) = (byte)dato_in;
}

void EEPROM_I2C_Lectura_datos() 
{
  eeprom_mem = 0;
  byte paquete[30];
  float aux[7];
  uint16_t tim;
  for (int i = 0; i < 10000; i++) 
  {
    // Leer paquetes individuales
    for (int j = 0; j < 30; j++) 
    {
      paquete[j] = readEEPROM(j + eeprom_mem);
      //Serial.print(paquete[j], HEX);
    }
    eeprom_mem += 30;

    tim = paquete[1];
    tim += ((uint16_t)paquete[0]) << 8;
    Serial.print(tim);
    Serial.write('\t');

    for (int k = 0; k < 7; k++) 
    {
      _4byte_to_float(&(paquete[k * 4 + 2]), &(aux[k]));
      Serial.print(aux[k], 5);
      Serial.write('\t');
    }
    Serial.write('\n');

    if (eeprom_mem >= (65536 - 30)) 
    {
      Serial.println("Fin de la lectura de datos EEPROM_I2C");
      while (true);
    }
  }
  eeprom_mem = 0;
}

// Función para leer de la EEPROM
byte readEEPROM(uint32_t address) {
  byte rcvData = 0xFF;
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((uint32_t)(address >> 8));   // MSB
  Wire.write((uint32_t)(address & 0x00FF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
  rcvData =  Wire.read();
  return rcvData;
  delay(5);
}


// Función para escribir en la EEPROM
void writeEEPROM(uint16_t address, byte val) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((uint8_t)(address >> 8));   // MSB
  Wire.write((uint8_t)(address & 0xFF)); // LSB
  Wire.write(val);
  Wire.endTransmission();
  delay(5);
}
#endif
*/
