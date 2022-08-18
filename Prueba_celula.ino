#include "HX711.h"
#include "SD.h"

File file;
HX711 celula;


long lect;
long mes;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  celula.begin(A0, A1);
  while(!celula.is_ready()) {delay(1);}
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);

  SD.begin(53);
  //SD.remove("Datos");
  file = SD.open("Datos", FILE_WRITE);
}

void loop() 
{
  //lect = celula.read();
  //mes = (lect-5950)*3179.35/11875;
  //file.write((byte*)&lect,4);
  //file.write((byte*)&mes,4);
  // file.write(milis)`poner el tiempo
  
  //file.flush();
  //Serial.println(lect);
  lect = 0;
  for (int i=0; i<50 ; i++)
  {
  lect += celula.read(); 
  }
    Serial.println(lect/50); // NUESTRO CRO ES EN 8830
  
}
