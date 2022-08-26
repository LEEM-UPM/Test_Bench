//-------------------------------------------------
//-------------------------------------------------
//                     AVISOS
//-------------------------------------------------
//-------------------------------------------------

void error_inicio() 
{
  while (true) 
  {
   #if ALARMA == 1
    alarma_on();
   #endif
    digitalWrite(LED_ERROR, HIGH);
    delay(100);
   #if ALARMA == 1
    alarma_off();
   #endif
    digitalWrite(LED_ERROR, LOW);
    delay(100);
  }
}

#if RELE == 1
  void rele_cerrado()
  {
   digitalWrite(PIN_RELE, LOW);
  }
  
  void rele_abierto()
  {
   digitalWrite(PIN_RELE, HIGH);
  }
#endif
