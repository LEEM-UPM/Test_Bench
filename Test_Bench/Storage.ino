//-------------------------------------------------
//                DATA MANAGEMENT
//-------------------------------------------------

void pack_header()
{
  // Add header of every minipack, 2 ID bytes, and 20 length bytes
  miniPack[0] = serialID[0];
  miniPack[1] = serialID[1];
  miniPack[2] = serialID[1];
  miniPack[4] = 92;
}

void pack_change()
{
  // Time
  float_to_byte(sec, 5);
      
  // Cell data
  float_to_byte(cell_thrust, 9);

  // Transducer data  
  noInterrupts();
  float_to_byte(transducer_avg / transducer_counter, 13);
  interrupts();
  
  transducer_avg = 0;
  transducer_counter = 0;

  // DHT22 data
  float_to_byte(DHT_temp, 17);
  float_to_byte(DHT_hum, 21);

  //Temperature data
  for (int i = 0; i < 14; ++i) {
    for (int j = 0; j < 4; ++j) {
        miniPack[25 + i*4 + j] = temp_TP[i].byteP[j];
    }
  }
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
        miniPack[81 + i*4 + j] = temp_ADC[i].byteP[j];
    }
  }

  // CheckSum
  uint8_t check = 0;

  for (int i = 4; i < miniPackSize; ++i) 
    check += miniPack[i];
  miniPack[3] = check;
}

void float_to_byte(float var, int pos)
{
  byteConverter conv;
  conv.floatP = var;

  for (int i = 0; i < 4; ++i) 
    miniPack[pos + i] = conv.byteP[i];
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
    noInterrupts();
    RB_data.print(transducer_avg / transducer_counter, 5);
    RB_data.print(", ");
    interrupts();

    transducer_avg = 0;
    transducer_counter = 0;

    // DHT22 data
    RB_data.print(DHT_temp);
    RB_data.print(", ");

    RB_data.print(DHT_hum);

    // Temperature data
    for (int i = 0; i < 14; i++) {
      RB_data.print(", ");      
      RB_data.print(temp_TP[i].floatP);
    }
    for (int i = 0; i < 4; i++) {
      RB_data.print(", ");      
      RB_data.print(temp_ADC[i].floatP);
    }
    RB_data.println();   
  #endif
}

void file_pressure_update()
{
  #if TRANSDUCER == 1
    if (!SD_ready) {return;}

    // Transducer time (in s)
    float transducer_time = micros() / 10000000. - last_reset_micros;
    RB_pressure.print(transducer_time, 6);
    RB_pressure.print(", ");

    // Transducer measure already converted
    RB_pressure.print(transducer_pressure);
    RB_pressure.print(", ");

    // Raw transducer data    
    RB_pressure.println(transducer_raw);
  #endif   
}

bool file_open()
{
  #if SD_READER == 1
    if (!SD_ready) 
      return false;

    int i = 0; 
    String newName = i + file_data_name;

    while (SDF.exists(newName.c_str())) newName = ++i + file_data_name;

    if (!file_data.open(newName.c_str(), O_RDWR | O_CREAT))
    {
      send_order(errorSDFile);
      error_warning();
      SD_ready = false;
      return false;
    }

    #if TRANSDUCER == 1
      newName = i + file_pressure_name;
      if (!file_pressure.open(newName.c_str(), O_RDWR | O_CREAT))
      {
        send_order(errorSDFile);
        error_warning();
        SD_ready = false;
        return false;
      }
    #endif

    file_closed = false;

    // File header
    file_data.println("Time, Thrust, Chamber_Pressure, DHT_Temperature, DHT_Humidity, TP_temperature1, TP_temperature2, TP_temperature3, TP_temperature4, TP_temperature5,TP_temperature6, TP_temperature7, TP_temperature8, TP_temperature9, TP_temperature10, TP_temperature11, TP_temperature12, TP_temperature13, TP_temperature14, ADC_temperature1, ADC_temperature2, ADC_temperature3, ADC_temperature4");

    // Allocate space to avoid huge delays
    file_data.preAllocate(file_size); 

    // Starts the Ring Buffs
    RB_data.begin(&file_data);
    
    #if TRANSDUCER == 1
      file_pressure.println("Time, Chamber_Pressure, Raw_Transducer");
      file_pressure.preAllocate(file_size);
      RB_pressure.begin(&file_pressure);
    #endif
  #endif  

  return true;
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
