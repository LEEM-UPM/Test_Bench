//-------------------------------------------------
//                 COMMUNICATION
//-------------------------------------------------

uint8_t order_checking(uint8_t buf[], uint8_t length)
{
  Serial.println(length);
  // Size detection
  if (length < 7) 
    return -1;

  // ID detection
  if ((buf[0] != serialID[0]) || (buf[1] != serialID[1]) || (buf[2] != HERMES_ID))
    return -1;

  Serial.println("b");
  // Checksum 
  uint8_t checkSum = 0;
  for (int i = 4; i < length; ++i) 
    checkSum += buf[i];

  if (checkSum != buf[3])
    return -1;

  Serial.print("order :");
  Serial.println(buf[6]);  

  return buf[6];    
}

void check_BREDA_error (uint8_t error_byte)
{
  if (BREDA_error == error_byte)
    return;

  if ((error_byte & 1) == 1)
    send_order(errorSD + 100);
  else if ((error_byte & 2) == 2)
    send_order(errorSDFile + 100);
  else if ((error_byte & 4) == 4)
    send_order(errorFlash + 100);
  else if ((error_byte & 8) == 8)
    send_order(errorFlashFile + 100);   
    
  BREDA_error = error_byte;    
}

//-------------------------------------------------
//                I2C COMMUNICATION
//-------------------------------------------------

void send_BREDA_order(uint8_t order)
{
  Wire.beginTransmission(BREDA_ADDRESS);
  Wire.write(serialID, 2);
  Wire.write(BREDA_ID);
  Wire.write(order + 3);
  Wire.write(2);
  Wire.write(1);
  Wire.write(order);
  Wire.endTransmission();
}

void requestBreda()
{
  int count = 0;
  Wire.requestFrom(BREDA_ADDRESS, 84);
  while (Wire.available())
    Breda_buf[count++] = Wire.read();

  BREDA_order = order_checking(Breda_buf, count);

  // Reenvio de confirmacion de orden en caso de no haberla recibido bien
  if (BREDA_order == 255) 
  {
    return;
  }  
  else if ((order + 100) == BREDA_order) 
  {
    Serial.println("funciona: ");
    send_order(BREDA_order); // Sends BREDA order confirmation by telemetry (to differenciate boards we add 100)
    order = 0;
  }
  else if (order != 0)
  {
    send_BREDA_order(order);
  }

  for (int i = 0; i < 14; ++i) {
    for (int j = 0; j < 4; ++j) {
      temp_TP[i].byteP[j] = Breda_buf[12+(4*i+j)];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) { 
      temp_ADC[i].byteP[j] = Breda_buf[68+(4*i+j)];
    }
  }

  // Error byte
  check_BREDA_error(Breda_buf[7]);
}

