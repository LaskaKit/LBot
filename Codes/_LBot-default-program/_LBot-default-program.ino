#include <IRremote.h>
#include <Adafruit_NeoPixel.h>
#include <HCSR04.h>

const unsigned int cidlo_L = 9;  // Leve cidlo sledovani cary na pinu 9
const unsigned int cidlo_R = 10;  // Prave cidlo sledovani cary na pinu 10
int tdelay = 70;  // Prodleva pri zataceni v rezimu sledovani cary
bool sledovani_cary = false;  // Promenna, do ktere se bude ukladat zda je spusten rezim sledovani cary

bool bt = false;  // Promenna, do ktere se bude ukladat zda je aktivni ovladani pomoci bluetooth
char prijata_data;  // Promenna, do ktere se nactou prijata data z bluetooth

const unsigned int triggerPin = 11;  // Trigger pin ultrazvukoveho cidla na pinu D11
const unsigned int echoPin = 12;  // Echo pin ultrazvukoveho cidla na pinu D12
double vzdalenost;  // Promenna, do ktere se bude ukladat hodnota vzdalenosti z ultrazvukoveho cidla

const unsigned int bzucak = 8;  // Bzucak na pinu D8

const unsigned int tlacitko = A7;  // Tlacitko pripojeno na pin A7
bool stav_tlacitka = 0;  // Promenna, do ktere se bude ukladat stav tlacitka

const int svetelny_senzor = A6;  // Cidlo okolniho osvetleni na pinu A6
int osvetleni = 0;  // Promenna, do ktere se bude ukladat hodnota okolniho osvetleni

const unsigned int pwmLeft = 6; // PWM pin pro levy motor
const unsigned int left = 7; // Dir pin ktery urcuje smer pro levy motor
unsigned int korekce_L = 0;  // Promenna pro nastaveni korekce rychlosti leveho motoru
 
const unsigned int pwmRight = 5; // PWM pin pro pravy motor
const unsigned int right = 4; // Dir pin ktery urcuje smer pro pravy motor
unsigned int korekce_R = 0;  // Promenna pro nastaveni korekce rychlosti praveho motoru

unsigned int rychlost = 100;  // Rychlost jizdy v rozsahu 0 - 255, 0 = stop, 255 = rychlost 100%

const unsigned int ir_prijimac = 2;  // IR prijimac na pinu D2
decode_results prijaty_kod;  // Promenna, do ktere se bude ukladat prijaty kod z IR prijimace
unsigned long kod = 0;  // Pomocna promenna, do ktere se bude ukladat prijaty kod

bool p = 0;  // Pomocna promenna

IRrecv irrecv(ir_prijimac);  // Inicializace IR prijimace
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(2, 13, NEO_GRB + NEO_KHZ800);  // Inicializace RGB LED WS2812
UltraSonicDistanceSensor distanceSensor(triggerPin, echoPin);  // Inicializace ultrazvukoveho cidla

void setup() {
  Serial.begin(9600);  // Spusteni seriove komunikace
  pinMode(bzucak, OUTPUT);  // Pin pro bzucak jako vystup 
  pinMode(tlacitko, INPUT);  // Pin pro tlacitko jako vstup
  pinMode(pwmLeft, OUTPUT);  // PWM pin A jako vystup
  pinMode(left, OUTPUT);  // Dir pin A jako vystup
  pinMode(pwmRight, OUTPUT);  // PWM pin B jako vystup
  pinMode(right, OUTPUT);  // Dir pin B jako vystup
  pinMode(LED_BUILTIN, OUTPUT);  // Pin integrovane LED na Arduino NANO jako vystup
  
  irrecv.enableIRIn();  // Spusteni komunikace s IR prijimacem
  irrecv.blink13(true);  // Pri prijmu kodu z IR prijimace blika integrovana LED na Arduino NANO
  rgbWS.begin();  // Spusteni komunikace s RGB LED WS2812
  rgbWS.setBrightness(10);  // Nastaveni svitivosti RGB LED WS2812

  motoryStop();  // Zastavi motory
  
  Serial.println("Připraven...");
  
  nastavBarvu(0, 255, 0, 1);  // Nastavi barvu prvni RGB LED na zelenou
  nastavBarvu(0, 255, 0, 2);  // Nastavi barvu druhe RGB LED na zelenou
  pip();  // Pipne
  nastavBarvu(0, 0, 0, 1);  // Zhasne prvni RGB LED
  nastavBarvu(0, 0, 0, 2);  // Zhasne druhou RGB LED
}

void loop() {
  stav_tlacitka = !(analogRead(tlacitko) >= 512);  // Nacteni stavu tlacitka do promenne
  if (stav_tlacitka) {  // Pokud je tlacitko stisknuto
    pip();
    if (sledovani_cary){  // Pokud byl spusten rezim sledovani cary
      sledovani_cary = false;  // Vypni rezim sledovani cary
     Serial.println("Sledovani cary vypnuto");
    }else{  // Pokud nebyl spusten rezim sledovani cary
      sledovani_cary = true;  // Zapni rezim sledovani cary
     Serial.println("Sledovani cary zapnuto");
    }
    delay(250);
  }

  if (sledovani_cary){  // Pokud byl zapnut rezim sledovani cary
    rychlost = 70;  // Nastaveni rychlosti jizdy

    bool s1 = !digitalRead(cidlo_L);  // Nacteni logicke urvone z leveho cidla sledovani cary a jeji negace
    bool s2 = !digitalRead(cidlo_R);  // Nacteni logicke urvone z praveho cidla sledovani cary a jeji negace
 
    if (s1 && s2) {  // Pokud je logicka hodnota na obou cidlech H(1)
      jedDopredu(); 
      //Serial.println("Dopredu");
    } else if (!s1 && !s2) {  // Pokud je logicka hodnota na obou cidlech L(0)
      motoryStop(); 
      //Serial.println("Stop");
    } else if (s1 && !s2) {  // Pokud je logicka hodnota na levem cidle H(1) a na pravem cidle L(0)
      otocDoprava(); 
      //Serial.println("Doprava");
      delay(tdelay);
    } else if (!s1 && s2) {  // Pokud je logicka hodnota na levem cidle L(0) a na pravem cidle H(1)
      otocDoleva(); 
      //Serial.println("Doleva");
      delay(tdelay);
    }

    if (irrecv.decode(&prijaty_kod)){  // Poku je prijaty kod
      irrecv.resume();  // Nacte novou hodnotu
      
      switch (prijaty_kod.value) {
        case 16738455:
          Serial.println("Sledovani cary vypnuto");
          sledovani_cary = false;  // Vypne rezim sledovani cary
          motoryStop();
          p = 1;
          pip();
          break;
        default:
          //
          break;
      }
    }

    if (Serial.available() > 0){  // Poku jsou prijaty data z Bluetooth
      prijata_data = Serial.read();  // Nacte data z Bluetooth
      delay(2);
      
      switch (prijata_data) {
        case '2':  // Pokud z bluetooth prijde prikaz "2", vypne rezim sledovani cary
          Serial.println("Sledovani cary vypnuto");
          sledovani_cary = false;  // Vypne rezim sledovani cary
          motoryStop();
          pip();
          break;
        default:
          //
          break;
      } 
    }
  }else{  // Pokud byl vypnut rezim sledovani cary
  rychlost = 150;

  vzdalenost = distanceSensor.measureDistanceCm();  // Zmereni a ulozeni do promenne hodnoty vzdalenosti ultrazvukovym cidlem. Hodnota vzdalenosti je v cm
  if (vzdalenost < 0) vzdalenost = 0;  // Knihovna vraci -1, pokud je zmereny rozsah mimo pracovni oblasti cidla = < 0 a > 400cm
  //Serial.println(vzdalenost, 0);  

  if (irrecv.decode(&prijaty_kod)){  // Poku je prijaty kod
    irrecv.resume();  // Nacte novou hodnotu
    bt = false;  // Vypne rezim bluetooth
    
    if (prijaty_kod.value == 0xFFFFFFFF) prijaty_kod.value = kod;  // Pokud prichazi kod opakovane (pri delsim stisku tlacitka)
    //Serial.print(prijaty_kod.value, DEC);  // Vypise kod na seriovou konzoli v ciselne(desitkove) soustave
    //Serial.print(" - ");

    if ((vzdalenost <= 10) && (prijaty_kod.value == 16712445)){  // Pokud je vzdalenost od prekazky mensi, nebo rovna 10cm a prichazi kod pro jizdu vpred
      motoryStop();  // Zastavi motory
      delay(30);
      rychlost = 50;  // Snizeni rychlosti
      while (vzdalenost < 13){  // Bude couvat, dokud nebude vzdalenost od prekazky vetsi nez 12cm
        vzdalenost = distanceSensor.measureDistanceCm();  
        if (vzdalenost < 0) vzdalenost = 0;
        jedDozadu();
      }
      return;
    }

    switch (prijaty_kod.value) {
      case 16712445:
        jedDopredu();
        //Serial.println("Jedu dopredu");
        break;
      case 16750695:
        jedDozadu();
        //Serial.println("Jedu dozadu");
        break;
      case 16769055:
        otocDoleva();
        //Serial.println("Jedu doleva");
        break;
      case 16748655:
        otocDoprava();
        //Serial.println("Jedu doprava");
        break;
      case 16738455:
        if (!p){
          Serial.println("Sledovani cary zapnuto");
          sledovani_cary = true;  // Zapne rezim sledovani cary
          pip();
        }else{
          p = 0;
        }
        break;
      default:
        //Serial.println("Neznamy kod");
        break;
    }

    kod = prijaty_kod.value;  
  }else{
    if (!bt) motoryStop();
  }

  if (Serial.available() > 0){  // Poku jsou prijaty data z Bluetooth
    prijata_data = Serial.read();  // Nacte data z Bluetooth
    delay(2);

    switch (prijata_data) {
      case 'u':
        jedDopredu();
        bt = true;
        break;
      case 'd':
        jedDozadu();
        bt = true;
        break;
      case 'l':
        otocDoleva();
        bt = true;
        break;
      case 'r':
        otocDoprava();
        bt = true;
        break;
      case 'o':
        motoryStop();
        bt = false;
        break;
      case '2':  // Pokud prijde z bluetooth kod "2"
          Serial.println("Sledovani cary zapnuto");
          sledovani_cary = true;  // Zapne rezim sledovani cary
          pip();
          break;
      default:
        //
        break;
    } 
  }
    
  if ((vzdalenost <= 10) && bt){  // Pokud je vzdalenost od prekazky mensi, nebo rovna 10cm a je aktivni prikaz z bluetooth
    bt = false;  // Vypne rezim bluetooth
    motoryStop();  // Zastavi motory
    delay(30);
    rychlost = 50;  // Snizi rychlost
    while (vzdalenost < 13){  // Bude couvat, dokud nebude vzdalenost od prekazky vetsi nez 12cm
      vzdalenost = distanceSensor.measureDistanceCm();  
      if (vzdalenost < 0) vzdalenost = 0;
      jedDozadu();
    }
    motoryStop();
  }

  osvetleni = analogRead(svetelny_senzor);  // Nacteni hodnoty na analogovem pinu, kde je pripojen senzor. Hodnota v rozsahu 0 - 1024
  //Serial.println(osvetleni);
  if (osvetleni < 500){  // Pokud je hodnota okolniho osvetleni mensi nez 500
    nastavBarvu(255, 255, 255, 1);  // Rozsviti prvni RGB LED bilou barvou
    nastavBarvu(255, 255, 255, 2);  // Rozsviti druhou RGB LED bilou barvou
  }else{
    nastavBarvu(0, 0, 0, 1);  // Zhasne prvni RGB LED
    nastavBarvu(0, 0, 0, 2);  // Zhasne prvni RGB LED
  }
  delay(100);
  }
}

// Pipnuti s frekvenci 1kHz
void pip(){
  tone(bzucak, 1000);
  delay(200);       
  noTone(bzucak);     
}

// Pomocna procedura pro nastaveni barvy RGB LED WS2812
void nastavBarvu(byte r, byte g, byte b, int id) {  // r = hodnota cervene, g = hodnota zelene, b = hodnota modre, id = cislo LED v poradi, kterou budeme nastavovat(1 = 1. LED, 2 = 2. LED atd.)
  uint32_t rgb;  
  rgb = rgbWS.Color(r, g, b); // Konverze vstupnich hodnot R, G, B do pomocne promenne  
  rgbWS.setPixelColor(id - 1, rgb); // Nastavi pozadovanou barvu pro konkretni led = pozice LED zacinaji od nuly
  rgbWS.show();  // Zaktualizuje barvu
}

// Pomocna procedura pro vypocet korigované rychlosti otaceni leveho motoru 
unsigned int speed_L(unsigned int rychlost){
  unsigned int rychlost_L = rychlost + map(korekce_L, 0, 100, 0, rychlost);  // Vypocet a ulozeni do pomocne promenne korigovane hodnoty rychlosti
  if (rychlost_L > 255) rychlost_L = 255;
  if (rychlost_L < 0) rychlost_L = 0;
  return rychlost_L;
}

// Pomocna procedura pro vypocet korigované rychlosti otaceni praveho motoru
unsigned int speed_R(unsigned int rychlost){
  unsigned int rychlost_R = rychlost + map(korekce_R, 0, 100, 0, rychlost);  // Vypocet a ulozeni do pomocne promenne korigovane hodnoty rychlosti
  if (rychlost_R > 255) rychlost_R = 255;
  if (rychlost_R < 0) rychlost_R = 0;
  return rychlost_R;
}

// Zastavi motory
void motoryStop() {
  digitalWrite(left, LOW); // Dir pin motoru A na log. 0
  digitalWrite(right, LOW);  // Dir pin motoru B na log. 0
  analogWrite(pwmLeft, 0);  // PWM motoru A na 0%
  analogWrite(pwmRight, 0);  // PWM motoru B na 0%
}

// Jizda dopredu
void jedDopredu() {
  korekce_L = 7;  // Korekce rychlosti leveho motoru 7% nastavene rychlosti
  korekce_R = 0;
  unsigned int rychlost_L = speed_L(rychlost);  // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni leveho kola
  unsigned int rychlost_R = speed_R(rychlost);  // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni praveho kola
  digitalWrite(left, HIGH);  // Dir pin motoru A na log. 1
  digitalWrite(right, HIGH);  // Dir pin motoru B na log. 1
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Jizda dozadu
void jedDozadu() {
  korekce_L = 2;  // Korekce rychlosti leveho motoru 2% nastavene rychlosti
  korekce_R = 0;
  unsigned int rychlost_L = speed_L(rychlost);  // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni leveho kola
  unsigned int rychlost_R = speed_R(rychlost);  // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni praveho kola
  digitalWrite(left, LOW);  // Dir pin motoru A na log. 0
  digitalWrite(right, LOW);  // Dir pin motoru B na log. 0
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Otoci doleva
void otocDoleva() {
  unsigned int rychlost_L = map(50, 0, 100, 0, speed_L(rychlost));  // Nacteni a ulozeni do pomocne promenne 50% rychlosti leveho kola
  unsigned int rychlost_R = speed_R(rychlost); // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni praveho kola
  digitalWrite(left, LOW);  // Dir pin motoru A na log. 0
  digitalWrite(right, HIGH);  // Dir pin motoru B na log. 1
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Otoci doprava
void otocDoprava() {
  unsigned int rychlost_L = speed_L(rychlost);  // Nacteni a ulozeni do pomocne promenne udaj o rychlosti otaceni leveho kola
  unsigned int rychlost_R = map(50, 0, 100, 0, speed_R(rychlost));  // Nacteni a ulozeni do pomocne promenne 50% rychlosti praveho kola
  digitalWrite(left, HIGH);  // Dir pin motoru A na log. 1
  digitalWrite(right, LOW);  // Dir pin motoru B na log. 0
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}
