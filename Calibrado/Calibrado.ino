#include "HX711.h"

// HX711 circuit wiring
#define HX_DATA                    14
#define HX_SCK                     15

HX711 scale;

void values()
{
  scale.set_scale(8000/550.);  
  scale.tare(250);
  while (true)
  {
    long reading = scale.get_units(1);
    Serial.println(reading/1000.);
  }
}

void setup() {
  Serial.begin(57600);
  scale.begin(HX_DATA, HX_SCK);
  values();
}

void loop() {

  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}
