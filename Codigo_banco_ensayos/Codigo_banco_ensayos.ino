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
#define MODO_TESTEO                1
#define DEBUG                      1
#define UDP                        0
#define SERIE                      0
#define LEER_EEPROM                0


//-------------------------------------------------
//               MODOS DE TESTEO
//-------------------------------------------------
#if MODO_TESTEO == 1
 #define ESCANER_I2C               0
 #define TESTEO_DE_DATOS           1 
 #define ESCRITURA_SD              1 
 #define CALIBRADO_CELULA          0 
 #define TESTEO_LED                0 
 #define TESTEO_RELE               0
#endif

//-------------------------------------------------
//           SENSORES, MÓDULOS Y SISTEMAS
//-------------------------------------------------

#define PRESION_BMP280             1
#define HUMEDAD_DHT11              1
#define EEPROM_I2C                 0
#define LECTOR_SD                  1
#define CELULA_CARGA               1
#define TERMOPARES                 0
 #define TERM_1     1
 #define TERM_2     1
 #define TERM_3     1
 #define TERM_4     1
#define TEMPERATURA_DALLAS         0
 #define DALL_1     1
 #define DALL_2     1
 #define DALL_3     1
#define ACELEROMETRO               0
#define RELE                       0


//-------------------------------------------------
//              DECLARACIÓN DE PINES
//-------------------------------------------------

#define DHTPIN                     7
#define PIN_MICRO_SD_CS            53
#define LED_ERROR                  6
#define PIN_RELE                   8
#define pinDatosDQ                 9
#define PIN_RELE                   8

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
  #include <DallasTemperature.h>
#endif

// DHT11
#if HUMEDAD_DHT11 == 1
 #include <DHT.h>
#endif

// Acelerómetro
#if ACELEROMETRO == 1 
  #include "src/GyroNick_WT.h"
#endif

// Célula de carga
#if CELULA_CARGA == 1
 #include <HX711.h>
#endif

// Termopares 
#if TERMOPARES == 1
 #include <max6675.h>
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

// Termopares
#if TERMOPARES == 1
 #if TERM_1 == 1
  #define TERMOPAR1_TCSCK_PIN      24
  #define TERMOPAR1_TCCS_PIN       25
  #define TERMOPAR1_TCDO_PIN       26
  MAX6675 par1 (TERMOPAR1_TCSCK_PIN, TERMOPAR1_TCCS_PIN, TERMOPAR1_TCDO_PIN);
 #endif
 #if TERM_2 == 1
  #define TERMOPAR2_TCSCK_PIN      27
  #define TERMOPAR2_TCCS_PIN       28
  #define TERMOPAR2_TCDO_PIN       29
  MAX6675 par2 (TERMOPAR2_TCSCK_PIN, TERMOPAR2_TCCS_PIN, TERMOPAR2_TCDO_PIN);
 #endif
 #if TERM_3 == 1
  #define TERMOPAR3_TCSCK_PIN      30
  #define TERMOPAR3_TCCS_PIN       31
  #define TERMOPAR3_TCDO_PIN       32
  MAX6675 par3 (TERMOPAR3_TCSCK_PIN, TERMOPAR3_TCCS_PIN, TERMOPAR3_TCDO_PIN);
 #endif
 #if TERM_4 == 1
  #define TERMOPAR4_TCSCK_PIN      33
  #define TERMOPAR4_TCCS_PIN       34
  #define TERMOPAR4_TCDO_PIN       35
  MAX6675 par4 (TERMOPAR4_TCSCK_PIN, TERMOPAR4_TCCS_PIN, TERMOPAR4_TCDO_PIN);
 #endif
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

#if RELE == 1
  void rele_cerrado();
  void rele_abierto();
#endif

// Funciones generales
 void error_inicio();
 void inicio_sensores_y_modulos();
 void toma_de_datos();
 void visualizacion_datos_serie();
 void escritura_sd();
 void testeo_led();

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
  #if TESTEO_RELE == 1
   void testeo_rele();
  #endif
#endif

// Funciones Modo Ensayo
#if MODO_ENSAYO == 1
  void pre_ensayo();
  void ensayo();
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

// Variables DS18B20
#if TEMPERATURA_DALLAS == 1
 #if DALL_1 == 1
  float dallas_1;
 #endif
 #if DALL_2 == 1
  float dallas_2;
 #endif
 #if DALL_3 == 1
  float dallas_3;
 #endif
#endif

// Variables Termopares
#if TERMOPARES == 1
 #if TERM_1 == 1
  float termopar_1;
 #endif
 #if TERM_2 == 1
  float termopar_2;
 #endif
 #if TERM_3 == 1
  float termopar_3;
 #endif
 #if TERM_4 == 1
  float termopar_4;
 #endif
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

// RELÉ
#if RELE == 1
 pinMode(PIN_RELE, OUTPUT);
#endif

// Lectura de datos I2C
#if LEER_EEPROM == 1
  digitalWrite(LED_READY, HIGH);
  EEPROM_I2C_Lectura_datos();
  while (true)
   {
    delay(1);
   }
#endif

// Inicio y verificación célula de carga
#if CELULA_CARGA == 1
 celula.begin(A0, A1);
   if (!celula.is_ready())
  {
   #if DEBUG == 1
    Serial.print("Error al iniciar la célula de carga");
   #endif
   error_inicio();
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
  #if CALIBRADO_CELULA == 1
   calibrado_celula();
  #endif
  #if TESTEO_LED == 1
   testeo_led();
  #endif
  #if TESTEO_RELE == 1
   testeo_rele();
  #endif
#endif



//-------------------------------------------------
//                  Modo Ensayo
//-------------------------------------------------
#if MODO_ENSAYO == 1
 pre_ensayo();
#endif


}




//-------------------------------------------------
//-------------------------------------------------
//                 INICIO LOOP
//-------------------------------------------------
//-------------------------------------------------

void loop() 
{

//-------------------------------------------------
//                  Modo Testeo
//-------------------------------------------------

#if MODO_TESTEO == 1
  #if TESTEO_DE_DATOS == 1
   testeo_de_datos();
  #endif
#endif



//-------------------------------------------------
//                  Modo Ensayo
//-------------------------------------------------
#if MODO_ENSAYO == 1
 ensayo();
#endif








delay(50);// sustituir por el de abajo al acabar el código
/*
if (millis() - tiempo < 10)
  {
    delay(10 - millis() + tiempo); //delay de lo que falta para llegar a 10
  }
*/
}
