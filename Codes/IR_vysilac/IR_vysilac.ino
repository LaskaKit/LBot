#include <IRremote.h>

IRsend irsend;  // Inicializace vysilace

const unsigned int tlacitko = A7;
bool stav_tlacitka = 0;

void setup() {
  
}

void loop() {
  stav_tlacitka = !(analogRead(tlacitko) >= 512);
  if (stav_tlacitka) {
    unsigned long tData = 0x41; // Data pro odeslani – ASCII znak A
    irsend.sendSony(tData, 16);  // Odesleme data s kodovanim SONY – data, pocet_bitu_k_odeslani
    delay(200);
  }
}