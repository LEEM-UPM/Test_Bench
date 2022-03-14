// Firmware banco de ensayos v1.4
// Autores: Carlos, Nick y Andrés LEEM 21/22
// https://www.youtube.com/watch?v=_wDHf5tPUTg
// v1.4 Pirmeras purebas paquetes segudos

// Librerías oficiales:
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


// Librerías a instalar en el administrador de librerías:
#include <OneWire.h>

// Librerías externas
#include "src/GyroNick_WT.h"
#include "src/DHT.h"
#include "src/HX711.h"
#include "src/DallasTemperature.h"
#include "src/Adafruit_Sensor.h"
#include "src/Adafruit_BMP280.h"  // Entrar en Adafruit_BMP280.h y cambiar la dirección a 0x76



// -----------------------------------------------------------------
// --------------------------- SENSORES ----------------------------
// -----------------------------------------------------------------


// Acelerómetro/giroscipio WT901 (I2C)
VRU WT_TTL(&Serial);
VRU WT_I2C(0x50);


// Temperatura
#define pinDatosDQ      5   //PWM
OneWire oneWireObjeto(pinDatosDQ);
DallasTemperature sensorDS18B20(&oneWireObjeto);


// BMP                           (I2C)
Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();


// Célula de carga:
#define LOADCELL_DOUT_PIN  2
#define LOADCELL_SCK_PIN   3
HX711 scale;

const float MAX_LOAD = 300.0;
const float OFFSET = 1.83 - 0.30;
const float RESOLUTION = 40.0;   // mV
const long ADC_RES = 8388608;    // 2^24 / 2
const float FACTOR_GALGA = 2.0;  // mV/V
const float ALIMENTACION = 4.04;  // V

unsigned long count_a = 0;
unsigned long count_c = 0;


// DHT
#define DHTPIN 7          // Usar pines 3, 4, 5, 12, 13 or 14 --
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);


// Accesorios
#define PIN_BOTON 6
#define PIN_LED 4


// Tiempo
unsigned long t_o = 0;



// -----------------------------------------------------------------
// ----------------------------- UDP -------------------------------
// -----------------------------------------------------------------

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 176);
unsigned int localPort = 80;

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;


float to = 0.0;


// -----------------------------------------------------------------
// ----------------------------- CRC -------------------------------
// -----------------------------------------------------------------
// https://www.luisllamas.es/arduino-checksum/

uint8_t XORChecksum8(const byte *data, size_t dataLength)
{
  uint8_t value = 0;
  for (size_t i = 0; i < dataLength; i++)
  {
    value ^= (uint8_t)data[i];
  }
  return ~value;
}





void setup() {

  // Comunicaciones:
  Wire.setClock(100000);
  Ethernet.begin(mac, ip);
  Serial.begin(9600);

  // start UDP
  Udp.begin(localPort);
  Serial.println("Start");

  // Inicializar sensores:
  //scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 32);
  //bmp.begin();
  //sensorDS18B20.begin();
  //dht.begin();

  /* Default settings from datasheet. */
  //bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
  //                Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
  //                Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
  //                Adafruit_BMP280::FILTER_X16,      /* Filtering. */
  //                Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_LED, 1);
  delay(1000);
  digitalWrite(PIN_LED, 0);


  // Esperar conexión con matlab
  while (Udp.parsePacket() <= 0) {
  }
  Serial.println("Recibiendo...");
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.print("READY\n");
  Udp.endPacket();

}




void loop() {

  // Esperar a iniciar el ensayo
  while (true) {
    if (Udp.parsePacket() >= 5) {
      byte identificador[5];
      byte codigo[5] = {'S', 'T', 'A', 'R', 'T'};
      byte ok = 0;
      Udp.read(identificador, sizeof(identificador));

      for (int i = 0; i < 5; i++) {
        ok += codigo[i] == identificador[i];
      }

      if (ok == 5) {
        break;
      }

    }
  }

  digitalWrite(PIN_LED, 1);
  delay(1000);
  digitalWrite(PIN_LED, 0);

  to = (float)millis();
  
  for(int i = 0; i < 1000; i++){
    prueba_sin_sensores();
    //delay(10);
  }

  // Ensayo
  //Test_paquetes();

}





void lectura_lenta_test() {
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());

  byte Paquete[80];

  Paquete[0] = 'S';
  Paquete[1] = 'T';

  float t = ((float)millis() - to);
  t = t*0.001f;
  almacena_dato_float(&t, &(Paquete[2]), 't');


  lectura_acc_x(&(Paquete[7]));
  lectura_acc_y(&(Paquete[12]));
  lectura_acc_z(&(Paquete[17]));
  lectura_ang_x(&(Paquete[22]));
  lectura_ang_y(&(Paquete[27]));
  lectura_ang_z(&(Paquete[32]));
  lectura_t_wt(&(Paquete[37]));
  lectura_temp(&(Paquete[52]));
  lectura_BME(&(Paquete[57]));
  lectura_DHT11(&(Paquete[67]));
  lectura_load_cell(&(Paquete[72]));
  Paquete[77] = XORChecksum8(Paquete, 77);
  Paquete[78] = 'E';
  Paquete[79] = 'N';

  Udp.write(Paquete, sizeof(Paquete));
  Udp.endPacket();
}


void prueba_sin_sensores() {
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());

  byte Paquete[79];

  Paquete[0] = 'S';
  Paquete[1] = 'T';

  float t = ((float)millis() - to);
  float fa = f1(t);
  float fb = f2(t);

  t = t*0.001f;
  almacena_dato_float(&t, &(Paquete[2]), 't');

  float fc = analogRead(A0);
  
  almacena_dato_float((&fc), &(Paquete[7]), 'a');
  almacena_dato_float((&fa), &(Paquete[12]), 'b');
  almacena_dato_float((&fa), &(Paquete[17]), 'c');
  almacena_dato_float((&fa), &(Paquete[22]), 'd');
  almacena_dato_float((&fb), &(Paquete[27]), 'e');
  almacena_dato_float((&fb), &(Paquete[32]), 'f');
  almacena_dato_float((&fb), &(Paquete[37]), 'g');
  almacena_dato_float((&fb), &(Paquete[42]), 'h');
  almacena_dato_float((&fb), &(Paquete[47]), 'i');
  almacena_dato_float((&fa), &(Paquete[52]), 'j');
  almacena_dato_float((&fa), &(Paquete[57]), 'k');
  almacena_dato_float((&fa), &(Paquete[62]), 'l');
  almacena_dato_float((&fa), &(Paquete[67]), 'm');
  almacena_dato_float((&fa), &(Paquete[72]), 'n');
  Paquete[77] = 'E';
  Paquete[78] = XORChecksum8(Paquete, 77);
  

  Udp.write(Paquete, sizeof(Paquete));
  Udp.endPacket();
}

float f1(unsigned long t) {
  return 90 * (1 - exp(-(float)t / 10000.0));
}

float f2(unsigned long t) {
  return 90 * exp(-(float)t / 10000.0);
}


void lectura_load_cell(byte* dir) {
  // Leer sensor
  float force = 0.0;
  if (scale.is_ready()) {
    long reading = scale.read();
    float voltaje = ((float)(reading * RESOLUTION)) / ADC_RES;
    force = (voltaje * (MAX_LOAD / (FACTOR_GALGA * ALIMENTACION)) * 1.5) - OFFSET;
  }
  almacena_dato_float((&force), dir, 'a');
}


void lectura_acc_x(byte* dir) {
  WT_I2C.readI2C();
  almacena_dato_float((&(WT_I2C.acc[0])), dir, 'b');
}
void lectura_acc_y(byte* dir) {
  almacena_dato_float((&(WT_I2C.acc[1])), dir, 'c');
}
void lectura_acc_z(byte* dir) {
  almacena_dato_float((&(WT_I2C.acc[2])), dir, 'd');
}

void lectura_ang_x(byte* dir) {
  almacena_dato_float((&(WT_I2C.angle[0])), dir, 'e');
}
void lectura_ang_y(byte* dir) {
  almacena_dato_float((&(WT_I2C.angle[1])), dir, 'f');
}
void lectura_ang_z(byte* dir) {
  almacena_dato_float((&(WT_I2C.angle[2])), dir, 'g');
}

void lectura_t_wt(byte* dir) {
  almacena_dato_float(&(WT_I2C.temp), dir, 'h');
}

void lectura_temp(byte* dir) {
  sensorDS18B20.requestTemperatures();

  float dato = sensorDS18B20.getTempCByIndex(0);
  almacena_dato_float((&dato), dir, 'i');
  dato = sensorDS18B20.getTempCByIndex(1);
  almacena_dato_float((&dato), dir, 'j');
  dato = sensorDS18B20.getTempCByIndex(2);
  almacena_dato_float((&dato), dir, 'k');
}

void lectura_BME(byte* dir) {
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  almacena_dato_float((&(temp_event.temperature)), dir, 'l');
  almacena_dato_float((&(pressure_event.pressure)), dir, 'm');
}

void lectura_DHT11(byte* dir) {
  float dato = dht.readHumidity();
  almacena_dato_float((&dato), dir, 'n');
}

void almacena_dato_float(float* dir_dato, byte* dir, char identificador) {

  byte* puntero_dato = (byte*)dir_dato;
  *dir = identificador;
  dir++;
  for (byte i = 4; i > 0; i--) {
    *dir = *(puntero_dato);
    puntero_dato++;
    dir++;
  }
}
