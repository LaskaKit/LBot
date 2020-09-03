#include <HCSR04.h>  // Nacteni knihovny pro praci s ultrazvukovym cidlem

const unsigned int triggerPin = 11;  // Trigger pin cidla na pinu D11
const unsigned int echoPin = 12;  // Echo pin cidla na pinu D12

double vzdalenost;  // Promenna, do ktere se bude ukladat zmerena vzdalenost

UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);  // Inicializace cidla

void setup () {
  Serial.begin(115200);  // Spusteni seriove komunikace
}

void loop () {
  vzdalenost = distanceSensor.measureDistanceCm();  // Zmereni a ulozeni hodnoty vzdalenosti do promenne. Hodnota vzdalenosti je v cm
  if (vzdalenost < 0) vzdalenost = 0;  // Knihovna vraci -1, pokud je zmereny rozsah mimo pracovni oblasti cidla = < 0 a > 400cm
  Serial.println(distance, 0);  // Vypise na seriovou konzoli hodnotu zmerene vzdalenosti zaokrouhlenou na cele cislo
    
  delay(500);
}