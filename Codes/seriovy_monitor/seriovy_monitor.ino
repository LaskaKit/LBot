int i;

void setup() {
  Serial.begin(115200);  // Nastaveni a spusteni seriove komunikace s prenosovou rychlosti 115200baud
}

void loop() {
  
  Serial.print("Hodnota promenne i: ");  // Vypise text na sériovou konzoli, ale neposila konec radku – dalsi údaje se vypisou na stejny radek za tento text
  Serial.println(i);  // Vypsani hodnoty promenne i a ukonceni radku – dalsi udaje se budou vypisovat na novy radek

  i++; // Zvetseni hodnoty promenne i o 1
  
  delay(500); // Prodleva 0,5s pred dalsim opakovanim
}