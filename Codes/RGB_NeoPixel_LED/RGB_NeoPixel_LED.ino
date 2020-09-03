#include <Adafruit_NeoPixel.h>  // Nacteni knihovny

Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(2, 13, NEO_GRB + NEO_KHZ800);  // Inicializace LED – pocet_LED, pin_na_kterem_jsou_pripojeny_LED, parametry_komunikace

void setup() {
  rgbWS.begin();  // Spusteni komunikace s LED
  rgbWS.setBrightness(10);  // Nastaveni jasu LED – 0 = minimalni jas (nesviti), 255 = maximalni jas (sviti naplno)
}

void loop() {
  nastavBarvu(255, 0, 0, 1);  // Nastavi barvu 1. LED na cervenou
  nastavBarvu(0, 0, 255, 2);  // Nastavi barvu 2. LED na modrou
  delay(250);  // Prodleva 250ms
  nastavBarvu(0, 0, 255, 1);  // Nastavi barvu 1. LED na modrou
  nastavBarvu(255, 0, 0, 2);  // Nastavi barvu 2. LED na cervenou
  delay(250);  // Prodleva 250ms
}

void nastavBarvu(byte r, byte g, byte b, int id) {  // r = hodnota cervene, g = hodnota zelene, b = hodnota modre, id = cislo LED v poradi, kterou budeme nastavovat(1 = 1. LED, 2 = 2. LED atd.)
  uint32_t rgb;  // Pomocna promenna, do ktere se bude ukladat hodnota barvy
  
  rgb = rgbWS.Color(r, g, b); // Konverze vstupnich hodnot R, G, B do pomocne promenne
  
  rgbWS.setPixelColor(id - 1, rgb); // Nastavi pozadovanou barvu pro konkretni led = pozice LED zacinaji od nuly
  rgbWS.show();  // Zaktualizuje barvu
}