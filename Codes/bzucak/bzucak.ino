const unsigned int bzucak = 8;  // Bzucak pripojen na pin D8

void setup() {
  pinMode(bzucak, OUTPUT);  // Nastaveni pinu, kam je bzucak pripojen, jako vystupni

  tone(bzucak, 1000); // Spusteni bzucaku s frekvenci 1kHz
  delay(1000);  // Pauza 1s        
  noTone(bzucak);     // Zastaveni bzucaku
}

void loop() {
  
}