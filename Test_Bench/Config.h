#ifndef HEADER_H
#define HEADER_H

//-------------------------------------------------
//              SENSORS AND MODULES
//-------------------------------------------------

#define TRANSDUCER                 1
#define FREQD                      0
#define RADIO                      0
#define BMP_280                    0
#define SD_READER                  1
#define W_CELL                     1
#define DHT_22                     1
#define RELAY                      1

//-------------------------------------------------
//                      PINS
//-------------------------------------------------

#define XBEE_COM                   Serial8
#define PIN_ALARM                  2
#define PIN_RELAY                  3
#define PIN_DHT                    4
#define PIN_LED                    13
#define HX_DOUT                    14
#define HX_SCK                     15
#define PIN_TRANSDUCER             A2

//-------------------------------------------------
//                   LIBRARIES
//-------------------------------------------------

// Wire
#include <Wire.h>

// DHT22
#include <DHT.h>

// BMP280
#include <Adafruit_BMP280.h>

// Weight cell
#include <HX711.h>

// SD
#include <SdFat.h>
#include <RingBuf.h>

//-------------------------------------------------
//                   VARIABLES
//-------------------------------------------------

// ID Bytes
const uint8_t serialID[] = {0xFE, 0xFB};

// Time
float sec = 0;
uint32_t time_rn = 0;
uint32_t last_time = 0;
uint32_t last_reset = 0;
uint32_t last_cell_freq = 0;
uint32_t last_transducer_freq = 0;
uint32_t last_transducer = 0;
uint32_t last_DHT = 0;
uint32_t last_LED = 0;
uint32_t last_ignition = 0;
uint32_t last_alarm = 0;

// Timers
const uint32_t transducer_timer = 100;
const uint32_t DHT_timer = 1000;
const uint32_t ignition_timer = 5000;
const uint32_t LED_timer = 1000;
const uint32_t alarm_timer = 1000;

// Serial
const uint16_t serialSize = 500;

uint8_t order = 0;
uint8_t serialBuffer[serialSize] = "";
uint16_t length = 0;

// Pack deliver
const uint16_t packSize = 4;
const uint16_t miniPackSize = 29;

uint16_t wholePackSize = miniPackSize * packSize;
uint16_t packPos = 0;
byte miniPack[miniPackSize] = {0};
byte wholePack[miniPackSize * packSize] = {0};

// SD
const uint32_t RB_size = 400 * 512;
const uint32_t file_size = 10 * 25000 * 600;

bool file_closed = true;
bool SD_ready = true;
String file_data_name = "_Datos.txt";
String file_pressure_name = "_Presion.txt";

// Modes
bool receiving = false;
bool tared = false;
bool performance_started = false;

// Cell Variables
const float cell_f = (5*2.5)/(1024*175);

uint16_t cell_freq = 0;
float cell_thrust = 0;

// Transducer variables
const float transducer_max_pressure = 343.7 / 200;
const float transducer_max_voltage = 620;

bool transducer_enabled = true;
uint16_t transducer_raw = 0;
uint16_t transducer_counter = 0;
uint16_t transducer_freq = 0;
float transducer_offset = 0;
float transducer_avg = 0;
float transducer_pressure = 0;

// BMP280 variables
float BMP_temp = 0;
float BMP_pressure = 0; 
float BMP_ref_pressure = 0;

// DHT22 variables
float DHT_hum = 0;
float DHT_temp = 0;

// Ignition variables
bool ignition_started = false;

// Led variables
bool LED_started = true;

// Alarm variables
bool alarm_status = false;

//-------------------------------------------------
//                  CONFIGURATION    
//-------------------------------------------------

// BMP280
#if BMP_280 == 1
  Adafruit_BMP280* bmp = new Adafruit_BMP280;
#endif

// DHT22
#if DHT_22 == 1
  DHT dht(PIN_DHT, DHT22);
#endif

// Cell
#if W_CELL == 1
  HX711 cell;
#endif

// SD
#if SD_READER == 1
  SdFs SDF;
  FsFile file_data;
  
  // RingBuf for File type FsFile.
  RingBuf<FsFile, RB_size> RB_data;

  #if TRANSDUCER == 1
    FsFile file_pressure;
    RingBuf<FsFile, RB_size> RB_pressure;
  #endif
#endif

//-------------------------------------------------
//                    FUNCTIONS   
//-------------------------------------------------

void sensor_init();
void iteration();
void performance();
void performance_finished();
void power_relay(bool);
void relay_warning();
void error_warning();
void transducer_set_offset();
void transducer_measure();
void data_measure();
void pack_header();
void pack_change();
void float_to_byte(float, int);
void file_data_update();
void file_pressure_update();
void file_close();
bool file_open();
void data_deliver();
void serial_read();
void send_order(uint8_t); 
void obey_order(uint8_t);

#endif