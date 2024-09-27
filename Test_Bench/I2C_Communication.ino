//-------------------------------------------------
//                 COMMUNICATION
//-------------------------------------------------

uint8_t order_checking(uint8_t buf[], uint8_t length)
{
  // Size detection
  if (length < 7) 
    return -1;

  // ID detection
  if ((buf[0] != serialID[0]) || (buf[1] != serialID[1]) || (buf[2] != HERMES_ID))
    return -1;

  // Checksum 
  uint8_t checkSum = 0;
  for (int i = 4; i < length; ++i) 
    checkSum += buf[i];

  if (checkSum != buf[3])
    return -1;

  return buf[6];    
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
  Wire.requestFrom(BREDA_ADDRESS, 7);

  while (Wire.available())
    I2C_buf[count++] = Wire.read();

  uint8_t BREDA_order = order_checking(buf, count);

  if (BREDA_order == 255) 
  {
    return;
  }  
  else if (BREDA_order == order) 
  {
    order = 0;
    send_order(BREDA_order + 100); // Sends BREDA order confirmation by telemetry (to differenciate boards we add 100)
  }
  else if (order != 0)
  {
    send_BREDA_order(order);
  }
}