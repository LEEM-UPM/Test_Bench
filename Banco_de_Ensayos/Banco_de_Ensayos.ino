//-------------------------------------------------
//-------------------------------------------------
//            CÓDIGO BANCO DE ENSAYOS
//-------------------------------------------------
//-------------------------------------------------


//-------------------------------------------------
//           SENSORES, MÓDULOS Y SISTEMAS
//-------------------------------------------------

#define TRANSDUCTOR                1
#define FREQD                      0
#define DEBUG                      0
#define TELEMETRIA                 1
#define BMP_280                    0
#define LECTOR_SD                  1
#define CELULA_CARGA               1
#define DHT_22                     0
#define RELE                       1

//-------------------------------------------------
//              DECLARACIÓN DE PINES
//-------------------------------------------------

#define XBEE_COM                   Serial8
#define PIN_RELE                   3
#define PIN_DHT                    4
#define PIN_LED                    13
#define HX_DOUT                    14
#define HX_SCK                     15
#define PIN_TRANSDUCTOR            A2

//-------------------------------------------------
//                   LIBRERÍAS
//-------------------------------------------------

// Wire
#include <Wire.h>

// DHT22
#include <DHT.h>

// BMP280
#include <Adafruit_BMP280.h>

// Célula de carga
#include <HX711.h>

// Termopares 
#include <max6675.h>

// Lector SD
#include <SD.h>

//-------------------------------------------------
//          CONFIGURACIÓN DE SENSORES    
//-------------------------------------------------

// BMP280
#if BMP_280 == 1
  Adafruit_BMP280* bmp = new Adafruit_BMP280;
#endif

// DHT22
#if DHT_22 == 1
  DHT dht(PIN_DHT, DHT22);
#endif

// Célula de carga
#if CELULA_CARGA == 1
  HX711 celula;
#endif

// Lector SD
#if LECTOR_SD == 1
  File archivo_datos;
  File archivo_presion;
#endif

//-------------------------------------------------
//                  FUNCIONES
//-------------------------------------------------

// Funciones generales
void inicio_sensores();
void error_inicio();
void rele(bool);

// Funciones paquete
union byteToFloat;
void encabezamiento_pack();
void cambio_pack();
void float_to_byte(float, int);

// Funciones datos
void toma_datos();
void serial_datos();
void envio_datos();

// Funciones comunicacion
void leer_serial();
void rec_orden(char);
void envio_orden(char);

// Funciones SD
bool abrir_archivo();
void escritura_datos_SD();
void escritura_presion_SD();

// Funciones Modo Ensayo
void iteracion();
void ensayo();

// Funciones Transductor
void set_offset_transductor();
void funciones_transductor();

//-------------------------------------------------
//                   VARIABLES
//-------------------------------------------------

// Bytes de Identificacion
byte serialID[] = {0xFE, 0xFB};

// Tiempop
unsigned long long tiempo = 0;
unsigned long long lastTime = 0;
unsigned long long lastFreq = 0;
unsigned long long lastReset = 0;
unsigned long long lastTransductor = 0;
float sec = 0;

// Serial
const int serialSize = 500;
char serialBuffer[serialSize] = "";
int length = 0;
byte order = 0;

// Packs para enviar
const int packSize = 4;
const int miniPackSize = 29;
int wholePackSize = miniPackSize * packSize;
int packPos = 0;
byte miniPack[miniPackSize] = {0};
byte wholePack[miniPackSize * packSize] = {0};

// SD
bool archivoIsClosed = true;
bool SDReady = true;
String data_name = "_Datos.txt";
String transductor_name = "_Presion.txt";

// Modos
bool receiving = false;
bool tared = false;
bool testMode = true;
bool ensayoStarted = false;

// Variables célula de carga
const float mult_celula = (5*2.5)/(1024*175);
int freq = 0;
float empuje = 0;

// Variables Transductor
const float max_p_transductor = 343.7 / 200;
const float max_v_transductor = 620;
float offset_transductor = 0;
float avg_transductor = 0;
float presion_transductor = 0;
long lectura_transductor = 0;
long counter_transductor = 0;

// Variables BMP280
float temperatura_BMP = 0;
float presion_BMP = 0; 
float presion_referencia = 0;

// Variables DHT22
float humedad_DHT22 = 0;
float temperatura_DHT22 = 0;
unsigned long long DHT22Since = 0;

// Variables Ignicion
bool ignitionStarted = false;
unsigned long long ignitionSince = 0;

// Variables Led
bool ledStarted = true;
unsigned long long ledSince = 0;

//-------------------------------------------------
//-------------------------------------------------
//                 INICIO SETUP
//-------------------------------------------------
//-------------------------------------------------

void setup() 
{
  // Led de control
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, true);

  // Comunicaciones
  Wire.begin();
  Serial.begin(115200);

  // Telemetría
  Serial8.begin(230400);

  // Rele
  pinMode(PIN_RELE, OUTPUT);

  // Inicio de sensores
  inicio_sensores();

  // Pin de la célula
  pinMode(HX_DOUT, INPUT);
}

//-------------------------------------------------
//-------------------------------------------------
//                 INICIO LOOP
//-------------------------------------------------
//-------------------------------------------------

void loop() 
{
  // Lee la instrucción si ha llegado
  leer_serial();

  // Hace una iteracion de ensayo
  iteracion();

  // Lee el transductor aparte, ya que este tiene una frecuencia altísima)
  #if TRANSDUCTOR == 1 
    if ((micros() - lastTransductor) > 1000)
    {
      funciones_transductor();
      lastTransductor = micros();
    }
  #endif  
  
  // Apaga el led 1 segundo despues de mandar la orden
  if (!ledStarted && (millis() - ledSince > 1000)) 
  {
    ledStarted = true;
    digitalWrite(PIN_LED, true);
  }
  
  // Apaga el rele 70 segundos despues de encenderse
  #if RELE == 1 
    if (ignitionStarted && (millis() - ignitionSince > 5000)) 
    {
      ignitionStarted = false;
      rele(false);
    }
  #endif  
}