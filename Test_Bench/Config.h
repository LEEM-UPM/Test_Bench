#ifndef CONFIG_H
#define CONFIG_H

//-------------------------------------------------
//              SENSORS AND MODULES
//-------------------------------------------------

#define USE_ADC_0
#define TRANSDUCER                 1
#define FREQD                      0
#define RADIO                      1
#define SD_READER                  1
#define W_CELL                     1
#define DHT_22                     1

//-------------------------------------------------
//                      PINS
//-------------------------------------------------

#if RADIO == 1
#define RADIO_OUT                   Serial8
#else 
#define RADIO_OUT                   Serial
#endif

#define BREDA_ADDRESS              0x35
#define HERMES_ADDRESS             0x39

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

// Weight cell
#include <HX711.h>

// SD
#include <SdFat.h>
#include <RingBuf.h>

// Transducer
#include <ADC.h>
#include <ADC_util.h>
#include <DMAChannel.h>
#include <AnalogBufferDMA.h>
#include <FreeStack.h>

//-------------------------------------------------
//                   VARIABLES
//-------------------------------------------------

union byteConverter
{
	float floatP;
  uint16_t intP;
	uint8_t byteP[4];
};

// ID Bytes
const uint8_t serialID[] = {0xFE, 0xFB};
const uint8_t BREDA_ID = 0xFC;
const uint8_t HERMES_ID = 0xFA;

// Time
float sec = 0;
uint32_t time_rn = 0;
uint32_t last_time = 0;
uint32_t last_reset_millis = 0;
uint32_t last_reset_micros = 0;
uint32_t last_cell_freq = 0;
uint32_t last_transducer_freq = 0;
uint32_t last_DHT = 0;
uint32_t last_LED = 0;
uint32_t last_ignition = 0;
uint32_t last_alarm = 0;
uint32_t last_transducer_offset = 0;
uint32_t last_radio = 0;

// Timers
const uint32_t transducer_timer = 10;
const uint32_t DHT_timer = 1000;
const uint32_t ignition_timer = 5000;
const uint32_t LED_timer = 1000;
const uint32_t alarm_timer = 1000;
uint32_t radio_timer = 1000;

// Serial
const uint16_t serialSize = 500;

uint8_t order = 0;
uint8_t BREDA_order = 0;
uint8_t serialBuffer[serialSize] = "";
uint16_t length = 0;

// Pack deliver
const uint16_t miniPackSize = 97;
byte miniPack[miniPackSize] = {0};

// SD
const uint32_t RB_size = 400 * 512;
const uint32_t file_size = 10 * 100000 * 600;

bool file_closed = true;
bool SD_ready = true;
String file_data_name = "_Datos.txt";
String file_pressure_name = "_Presion.txt";

// Modes
bool performance_status = false;
bool hydrostatic_enabled = false;

// Cell Variables
const float cell_f = (5*2.5)/(1024*175);

uint16_t cell_freq = 0;
float cell_thrust = 0;

// Transducer variables
const uint16_t transducer_max_pressure = 250;
const uint16_t transducer_min_value = 496;
const uint16_t transducer_max_value = 2482;
const float transducer_const = 0.1667; //float(transducer_max_pressure) / (transducer_max_value - transducer_min_value);

volatile uint32_t transducer_counter = 0;
volatile uint32_t transducer_counter_offset = 0;
volatile uint32_t transducer_freq = 0;

volatile int16_t transducer_raw = 0;
volatile float transducer_avg = 0;
volatile float transducer_pressure = 0;

volatile int32_t transducer_offset = 0;

bool transducer_enabled = true;
bool transducer_offset_activated = false;

// DHT22 variables
float DHT_hum = 0;
float DHT_temp = 0;

// Breda buffer
uint8_t Breda_buf[100] = {0};
byteConverter temp_TP[14];
byteConverter temp_ADC[4];
uint8_t BREDA_error = 0;

// Ignition variables
bool ignition_started = false;

// Led variables
bool LED_started = true;

// Alarm variables
bool alarm_status = false;

enum radio_orders 
{
  arduinoConnected = 1,
  enableTransducer,
  disableTransducer,
  enableHydroStatic,
  disableHydroStatic,
  startPerforming = 8,
  stopPerforming,
  startIgnition = 11,
  stopIgnition,
  tare,
  errorSD = 20,
  errorSDFile,
  errorFlash,
  errorFlashFile,
};

//-------------------------------------------------
//                  CONFIGURATION    
//-------------------------------------------------

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

// Transducer
#if TRANSDUCER == 1
  ADC *adc = new ADC();
#endif

#endif