#include <IRremote.h>

// Motor A, Leva strana
const unsigned int pwmLeft = 6; // PWM pin pro motor A
const unsigned int left = 7; // Dir pin, ktery urcuje smer pro motor A
unsigned int korekce_L = 0;  // Promenna pro nastaveni korekce rychlosti leveho motoru
 
// Motor B, Prava strana
const unsigned int pwmRight = 5; // PWM pin pro motor B
const unsigned int right = 4; // Dir pin, ktery urcuje smer pro motor B
unsigned int korekce_R = 0;  // Promenna pro nastaveni korekce rychlosti praveho motoru

unsigned int rychlost = 200;  // Rychlost jizdy v rozsahu 0 - 255, 0 = stop, 255 = rychlost 100%

void setup() {   
  pinMode(pwmLeft, OUTPUT);  // PWM pin A jako vystup
  pinMode(left, OUTPUT);  // Dir pin A jako vystup
  pinMode(pwmRight, OUTPUT);  // PWM pin B jako vystup
  pinMode(right, OUTPUT);  // Dir pin B jako vystup 

  motoryStop();  // Motory stop
}

void loop() {
  decode_results prijaty_kod;  // Nacteny kod z prijimace ulozime do promenne

  if (irrecv.decode(&prijaty_kod)){  // Poku je prijaty kod
    irrecv.resume();  // Nacte novou hodnotu
    
    if (prijaty_kod.value == 0xFFFFFFFF) prijaty_kod.value = kod;  // Pokud prichazi kod opakovane (pri delsim stisku tlacitka na ovladaci)

    switch (prijaty_kod.value) {  // Podle prijateho kodu ovladame motory
      case 16712445:
        jedDopredu();
        break;
      case 16750695:
        jedDozadu();
        break;
      case 16769055:
        otocDoleva();
        break;
      case 16748655:
        otocDoprava();
        break;
      default:
        //
        break;
    }

    kod = prijaty_kod.value;   
  }else{  // Pokud neprichazi zadny kod tlacitek dalkoveho ovladace, zastav motory
    motoryStop();
  }
}

// Pomocna procedura pro vypocet korigovane rychlosti otaceni leveho motoru 
unsigned int speed_L(unsigned int rychlost){
  unsigned int rychlost_L = rychlost + map(korekce_L, 0, 100, 0, rychlost);
  if (rychlost_L > 255) rychlost_L = 255;
  if (rychlost_L < 0) rychlost_L = 0;
  return rychlost_L;
}

// Pomocna procedura pro vypocet korigovane rychlosti otaceni praveho motoru
unsigned int speed_R(unsigned int rychlost){
  unsigned int rychlost_R = rychlost + map(korekce_R, 0, 100, 0, rychlost);
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
  unsigned int rychlost_L = speed_L(rychlost);  // Vypocita rychlost leveho motoru
  unsigned int rychlost_R = speed_R(rychlost);  // Vypocita rychlost praveho motoru
  digitalWrite(left, HIGH);  // Dir pin motoru A na log. 1
  digitalWrite(right, HIGH);  // Dir pin motoru B na log. 1
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Jizda dozadu
void jedDozadu() {
  korekce_L = 2;  // Korekce rychlosti leveho motoru 2% nastavene rychlosti
  korekce_R = 0;
  unsigned int rychlost_L = speed_L(rychlost);  // Vypocita rychlost leveho motoru
  unsigned int rychlost_R = speed_R(rychlost);  // Vypocita rychlost praveho motoru
  digitalWrite(left, LOW);  // Dir pin motoru A na log. 0
  digitalWrite(right, LOW);  // Dir pin motoru B na log. 0
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Otoci doleva
void otocDoleva() {
  unsigned int rychlost_L = map(50, 0, 100, 0, speed_L(rychlost));  // 50% rychlosti praveho kola
  unsigned int rychlost_R = speed_R(rychlost);
  digitalWrite(left, LOW);  // Dir pin motoru A na log. 0
  digitalWrite(right, HIGH);  // Dir pin motoru B na log. 1
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}

// Otoci doprava
void otocDoprava() {
  unsigned int rychlost_L = speed_L(rychlost);
  unsigned int rychlost_R = map(50, 0, 100, 0, speed_R(rychlost));  // 50% rychlosti leveho kola
  digitalWrite(left, HIGH);  // Dir pin motoru A na log. 1
  digitalWrite(right, LOW);  // Dir pin motoru B na log. 0
  analogWrite(pwmLeft, rychlost_L);  // PWM motoru A na nastavenou rychlost
  analogWrite(pwmRight, rychlost_R);  // PWM motoru B na nastavenou rychlost
}