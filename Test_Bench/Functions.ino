//-------------------------------------------------
//                 INITIALIZATION
//-------------------------------------------------

void sensor_init()
{
  bool error = false;

  power_relay(false);

  // BMP280 start
  #if BMP_280 == 1
    if (!bmp->begin(0x76))
    {
      send_order(12);
      error = true;
    }
  #endif

  // DHT start
  #if DHT_22 == 1
    dht.begin();
  #endif

  // Cell start
  #if W_CELL == 1
    cell.begin(HX_DOUT, HX_SCK);
    if (!cell.is_ready())
    {
      send_order(13);
      error = true;
    }
  #endif

  // SD Start
  #if SD_READER == 1
    if (!SDF.begin(SdioConfig(FIFO_SDIO))) 
    {
      send_order(14);
      error = true;
      SD_ready = false;
    }
  #endif

  // Transducer start
  #if TRANSDUCER == 1
    transducer_set_offset();
  #endif

  if (error) error_warning();

  pack_header();  
  wholePackSize = miniPackSize;
}

//-------------------------------------------------
//                   PERFORMANCE
//-------------------------------------------------

void iteration()
{
  if (digitalRead(HX_DOUT)) {return;}

  time_rn = millis();

  if (performance_started)
  {
    // Performance
    performance(); 
    last_time = time_rn;     

    // To know the frequency
    #if FREQD == 1 
      ++cell_freq; 
      if (time_rn - last_cell_freq > 1000)
      {
        Serial.println();
        Serial.print("Cell freq : ");
        Serial.println(cell_freq);
        cell_freq = 0;
        last_cell_freq = time_rn;
      }
    #endif    
  }   
  // If not performing, sends data one time per second 
  else if (time_rn - last_time >= 1000) 
  {
    last_time = time_rn;
    performance();
  } 
}

void performance() 
{
  // Measures sensors
  data_measure();
  
  // Sets the pack
  pack_change();

  // Sends every 4 minipacks
  for (int i = 0; i < miniPackSize; ++i) wholePack[i + packPos] = miniPack[i];
  packPos += miniPackSize;

  if (packPos == wholePackSize)
  {
    packPos = 0;
    data_deliver();  
  }  

  // Writes in SD
  file_data_update();
}

void performance_finished()
{
  #if SD_READER == 1
    if (!file_closed) {file_close();}
  #endif

  power_relay(false);
  performance_started = false;
  ignition_started = false;
  tared = false;
  wholePackSize = miniPackSize;
  packPos = 0;
  last_reset = millis();
}

//-------------------------------------------------
//                     RELAY
//-------------------------------------------------

void power_relay(bool status)
{
  digitalWrite(PIN_RELAY, status);
  digitalWrite(PIN_ALARM, status);
} 

//-------------------------------------------------
//                    WARNINGS
//-------------------------------------------------

void error_warning() 
{
  for (int i=0; i<10; i++)
  {
    digitalWrite(PIN_LED, false);
    delay(200);
    digitalWrite(PIN_LED, true);
    delay(200);
  }
}

//-------------------------------------------------
//                    TRANSDUCER
//-------------------------------------------------

void transducer_set_offset()
{
  // Read and get the average
  uint32_t transducer_sum = 0;

  for (int i = 0; i < 500; ++i)
  {
    transducer_sum += analogRead(PIN_TRANSDUCER);
    delay(1);
  }

  transducer_offset = transducer_sum / (transducer_max_voltage * 500) * 250;

}

void transducer_measure()
{
  // Read analog pin and change it to bars
  transducer_raw = analogRead(PIN_TRANSDUCER);

  transducer_pressure = (transducer_raw / transducer_max_voltage) * 250 - transducer_offset;
  
  transducer_avg += transducer_pressure;
  ++transducer_counter;

  // Write in SD
  if (performance_started) file_pressure_update();
}

//-------------------------------------------------
//                DATA MANAGEMENT
//-------------------------------------------------

void data_measure()
{
  // Time 
  sec = (millis() - last_reset) / 1000.;

  // BMP280
  #if BMP_280 == 1
    BMP_temp = bmp->readTemperature();
    BMP_pressure = bmp->readPressure();
  #endif

  // DHT
  #if DHT_22 == 1
    if (millis() - last_DHT > DHT_timer) 
    {
      DHT_hum = dht.readHumidity();
      DHT_temp = dht.readTemperature();
      last_DHT = millis();
    }
  #endif

  // Cell
  #if W_CELL == 1
    cell_thrust = cell_f * cell.get_value(1);
    if (cell_thrust < 0) cell_thrust = 0;
  #endif
}

union byteConverter
{
	float floatP;
  int intP;
	byte byteP[4];
};

void pack_header()
{
  // Add header of every minipack, 2 ID bytes, and 24 length bytes
  miniPack[0] = serialID[0];
  miniPack[1] = serialID[1];
  miniPack[4] = 0X18;
}

void pack_change()
{
  // Time
  float_to_byte(sec, 5);
      
  // Cell data
  float_to_byte(cell_thrust, 9);

  // Transducer data
  float true_pressure = transducer_avg / transducer_counter;
  float_to_byte(true_pressure, 13);
  transducer_avg = 0;
  transducer_counter = 0;

  // BMP280 data
  float_to_byte(BMP_temp, 17);
  float_to_byte(BMP_pressure, 21);

  // DHT22 data
  float_to_byte(DHT_hum, 25);

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
//                    SD WRITING
//-------------------------------------------------

void file_data_update()
{
  #if SD_READER == 1
    if (!SD_ready) {return;}

    // Time
    RB_data.print(sec, 3);
    RB_data.print(", ");
        
    // Cell data
    RB_data.print(cell_thrust, 5);
    RB_data.print(", ");

    // Transducer data (Average)
    float true_pressure = (transducer_avg / transducer_counter) * transducer_max_pressure;
    transducer_avg = 0;
    transducer_counter = 0;
    RB_data.print(true_pressure, 5);
    RB_data.print(", ");

    // BMP280 data
    RB_data.print(BMP_temp);
    RB_data.print(", ");
    RB_data.print(BMP_pressure);
    RB_data.print(", ");

    // DHT22 data
    RB_data.println(DHT_hum);

    // Write it in the round buffer
    size_t n = RB_data.bytesUsed();
    if (n >= 512 && !file_data.isBusy())
    {
      RB_data.writeOut(512);
    }
  #endif
}

void file_pressure_update()
{
  #if TRANSDUCER == 1
    if (!SD_ready) {return;}

    // Transducer time (in s)
    float transducer_time = (micros() / 1000. - last_reset) / 1000.;
    RB_pressure.print(transducer_time, 5);
    RB_pressure.print(", ");

    // Transducer measure already converted
    RB_pressure.print(transducer_pressure * transducer_max_pressure);
    RB_pressure.print(", ");

    // Raw transducer data
    RB_pressure.println(transducer_raw);

    // Write it in the round buffer
    size_t n = RB_pressure.bytesUsed();
    if (n >= 512 && !file_pressure.isBusy())
    {
      RB_pressure.writeOut(512);
    }
  #endif  
}

void file_close()
{
  #if SD_READER == 1
    // Empty the buffer, truncate the file size and close it
    RB_data.sync();
    file_data.truncate();
    file_data.close();

    #if TRANSDUCER == 1
      RB_pressure.sync();
      file_pressure.truncate();
      file_pressure.close();
    #endif

    file_closed = true;
  #endif  
}

bool file_open()
{
  #if SD_READER == 1
    if (!SD_ready) {return false;}

    int i = 0; 
    String newName = i + file_data_name;

    while (SDF.exists(newName.c_str())) newName = ++i + file_data_name;

    if (!file_data.open(newName.c_str(), O_RDWR | O_CREAT))
    {
      send_order(15);
      error_warning();
      SD_ready = false;
      return true;
    }

    #if TRANSDUCER == 1
      newName = i + file_pressure_name;
      if (!file_pressure.open(newName.c_str(), O_RDWR | O_CREAT))
      {
        send_order(15);
        error_warning();
        SD_ready = false;
        return true;
      }
    #endif

    file_closed = false;

    // Allocate space to avoid huge delays
    file_data.preAllocate(file_size); 

    // Starts the Ring Buffs
    RB_data.begin(&file_data);

    // File header
    file_data.println("Time, Thrust, Chamber_Pressure, BMP_Temperatura, BMP_Pressure, DHT_Humidity");

    #if TRANSDUCER == 1
      file_pressure.println("Time, Chamber_Pressure, Raw_Transducer");
      file_pressure.preAllocate(file_size);
      RB_pressure.begin(&file_pressure);
    #endif
  #endif  

  return false;
}
