#include <IRremote.h>

const unsigned int ir_prijimac = 2;  // IR prijimac na pinu D2
IRrecv irrecv(ir_prijimac);  // Inicializace prijimace
unsigned long kod = 0;  // Pomocna promenna, do ktere se bude ukladat prijaty kod

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);  // Pin integrovane LED na desce Arduino NANO jako vystup
  irrecv.enableIRIn();  // Start komunikace s prijimacem
  irrecv.blink13(true);  // Pri prijmu kodu blika integrovana LED na desce Arduino NANO
}

void loop() {
  decode_results prijaty_kod;  // Nastaveni promenne, do ktere se bude ukladat prijaty kod

  if (irrecv.decode(&prijaty_kod)){  // Poku je prijaty kod
    if (prijaty_kod.value == 0xFFFFFFFF) prijaty_kod.value = kod;  // Pokud prichazi kod opakovane (pri delsim stisku tlacitka) priradi do promenne kod predchozi
    Serial.println(prijaty_kod.value, DEC);  // Vypise kod na seriovou konzoli v ciselne(desitkove) soustave
    irrecv.resume();  // Nacte novou hodnotu
    kod = prijaty_kod.value;  // Priradi prijaty kod do promenne
    delay(150);  // Pauza 150ms
  }
}