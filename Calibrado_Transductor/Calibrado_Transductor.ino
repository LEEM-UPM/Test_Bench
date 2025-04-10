// Transducer
#include <ADC.h>
#include <ADC_util.h>

#define USE_ADC_0

ADC *adc = new ADC();

float total = 0;
uint16_t counter = 0;
uint32_t last_measure = 0;
uint32_t last_printed = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(A2, INPUT);

  adc->adc0->setAveraging(8);
  adc->adc0->setResolution(12); // set bits of resolution
  adc->adc0->startSingleRead(A2); // call this to setup everything before the Timer starts, differential is also possible
  adc->adc0->enableInterrupts(transducer_measure);
  adc->adc0->startTimer(1000); //frequency in Hz
}

void loop()
{
  if ((millis() - last_printed) > 1000)
  {
    Serial.println(total / counter);
    total = 0;
    counter = 0;
    last_printed = millis();
  }
}

void transducer_measure()
{
  total += adc->adc0->readSingle();
  ++counter;
}