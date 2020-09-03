const unsigned int svetelny_senzor = A6;  // Senzor pripojen na pin A6
int val = 0;  // Promenna, do ktere se bude ukladat hodnota okolniho osvetleni

void setup() {
  Serial.begin(115200);
}

void loop() {
  val = analogRead(svetelny_senzor);  // Nacteni hodnoty na analogovem pinu, kde je pripojen senzor. Hodnota v rozsahu 0 - 1024
  Serial.println(val);  // Vypsani hodnoty na seriovou konzoli
  delay(250);
}