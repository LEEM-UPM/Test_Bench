//-------------------------------------------------
//-------------------------------------------------
//            CÓDIGO BANCO DE ENSAYOS
//-------------------------------------------------
//-------------------------------------------------

// Lista de módulos y sensores del Banco de Ensayos:


//-------------------------------------------------
//               OPCIONES DEL BANCO
//-------------------------------------------------

#define ENSAYO                     0
#define DEBUG                      1
#define UDP                        0
#define SERIE                      0
#define LEER_EEPROM                0


//-------------------------------------------------
//               MODOS DE TESTEO
//-------------------------------------------------
#if MODO_TESTEO == 1
 #define ESCANER_I2C               0
 #define TESTEO_DE_DATOS           0
 #define ESCRITURA_SD              0
 #define CALIBRADO_CELULA          0
#endif

//-------------------------------------------------
//           SENSORES, MÓDULOS Y SISTEMAS
//-------------------------------------------------

#define PRESION_BMP280             1
#define HUMEDAD_DHT11              1
#define EEPROM_I2C                 0
#define LECTOR_SD                  0
#define CELULA_CARGA               0
#define RELE_IGNITOR               0
#define TERMOPARES                 0
#define TEMPERATURA_DALLAS         0
#define ACELEROMETRO               0


//-------------------------------------------------
//              DECLARACIÓN DE PINES
//-------------------------------------------------

#define DHTPIN                     7
#define PIN_MICRO_SD_CS            53
#define LED_ERROR                  6
#define PIN_RELE                   8
#define pinDatosDQ                 9


//-------------------------------------------------
//                   Librerías
//-------------------------------------------------

// Wire
 #include <Wire.h>

// BMP280
#if PRESION_BMP280 == 1
 #include <Adafruit_BMP280.h>
#endif

// DS18B20
#if TEMPERATURA_DALLAS == 1
  #include <OneWire.h>
#endif

// DHT11
#if HUMEDAD_DHT11 == 1
 #include <DHT.h>
 #include <DallasTemperature.h>
#endif

// Acelerómetro
#if ACELEROMETRO == 1 
  #include "src/GyroNick_WT.h"
#endif

// Célua de carga
#if CELULA_CARGA == 1
 #include <HX711.h>
#endif

// Lector SD
#if LECTOR_SD == 1
 #include <SD.h>
 #include <SPI.h>
#endif

// UDP
#if UDP == 1
 #include <Ethernet.h>
 #include <EthernetUdp.h>
#endif


//-------------------------------------------------
//    Configuración de los módulos y sensores        
//-------------------------------------------------

// BMP280
#if PRESION_BMP280 == 1
  Adafruit_BMP280* bmp = new Adafruit_BMP280;
#endif

// DHT11
#if HUMEDAD_DHT11 == 1
  #define DHTTYPE DHT11
  DHT  *dht = new DHT(DHTPIN, DHTTYPE);
#endif

// Célula de carga
#if CELULA_CARGA == 1
 File file;
 HX711 celula;
#endif

// DS18B20
#if TEMPERATURA_DALLAS == 1
 OneWire oneWireObjeto(pinDatosDQ);
 DallasTemperature sensorDS18B20(&oneWireObjeto);
#endif

// Acelerómetro
#if ACELEROMETRO == 1
 VRU WT_TTL(&Serial);
 VRU WT_I2C(0x50);
#endif

// Lector SD
#if LECTOR_SD == 1
  #define SSpin 53
  File *archivo;
#endif

// EEPROM I2C
#if EEPROM_I2C == 1
 #define EEPROM_I2C_ADDRESS 0x50
 uint16_t eeprom_mem = 0;
#endif


//-------------------------------------------------
//            Declaración de funciones
//-------------------------------------------------

// Funciones EEPROM I2C
#if EEPROM_I2C == 1
  boolean init_EEPROMI2C();
  void EEPROM_I2C_Almacena_datos();
#endif

// Funciones generales
 void error_inicio();
 void inicio_sensores_y_modulos();
 void toma_de_datos();
 void visualizacion_datos_serie();
 void escritura_sd();

// Funciones Modo Testeo
#if MODO_TESTEO == 1
  #if ESCANER_I2C == 1
   void escaner_i2c();
  #endif
  #if TESTEO_DE_DATOS == 1
   void inicio_de_datos();
   void testeo_de_datos();
  #endif
  #if CALIBRADO_CELULA == 1
   void calibrado_celula();
  #endif
#endif

 
//-------------------------------------------------
//               Configuración UDP
//-------------------------------------------------

#if UDP == 1
  byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  IPAddress ip(192, 168, 1, 176);
  unsigned int localPort = 80;
  
  // buffers for receiving and sending data
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
  char ReplyBuffer[] = "acknowledged";        // a string to send back
  
  // An EthernetUDP instance to let us send and receive packets over UDP
  EthernetUDP Udp;
#endif



//-------------------------------------------------
//                   Variables
//-------------------------------------------------

// Tiempo
unsigned long tiempo;

// Variables BMP280
#if PRESION_BMP280 == 1
  float temperatura_BMP;
  float altura_BMP;
  float presion_BMP;
  float presion_referencia;
#endif

// Variables DHT11
#if HUMEDAD_DHT11 == 1
  float humedad_DHT11;
  float temperatura_DHT11;
  unsigned int tiempo_DHT11;
#endif

// Variables célula de carga
#if CELULA_CARGA == 1
 long lect;
 long mes;
#endif



//-------------------------------------------------
//-------------------------------------------------
//                 INICIO SETUP
//-------------------------------------------------
//-------------------------------------------------

void setup() 
{

//---------------------------------------------------------
//  1.- Configuraciones iniciales
//---------------------------------------------------------

// Comunicaciones
 Wire.begin();
 Serial.begin(115200);
#if DEBUG == 1
 Serial.println("");
 Serial.println("");
#endif

// Inicio UDP
#if UDP == 1
 Ethernet.begin(mac, ip);
 Udp.begin(localPort);
#endif

// Led de control
pinMode(LED_ERROR, OUTPUT);

// Lectura de datos I2C
#if LEER_EEPROM == 1
  digitalWrite(LED_READY, HIGH);
  EEPROM_I2C_Lectura_datos();
  while (true)
   {
    delay(1);
   }
#endif

//-------------------------------------------------
//                  Modo Testeo
//-------------------------------------------------

#if MODO_TESTEO == 1
  #if ESCANER_I2C == 1
    escaner_i2c();
  #endif
  #if TESTEO_DE_DATOS == 1
   inicio_de_datos();
  #endif
#endif

//-------------------------------------------------
//                  Modo Ensayo
//-------------------------------------------------



}

void loop() 
{











  

}
