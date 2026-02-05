# Ugradeni-sustavi
Uvod
Razvoj sustava za pomoć pri parkiranju vozila ima važnu ulogu u povećanju sigurnosti i
preciznosti manevriranja u ograničenim prostorima. Cilj ovog projekta je izrada jednostavnog
parking senzora koji korisniku omogućuje procjenu udaljenosti vozila od prepreke bez potrebe
stalnog gledanja u zaslon. Sustav koristi kombinaciju zvučnog i vizualnog upozorenja kako bi
vozaču pružio jasnu i pravovremenu informaciju.
Opis sustava
Sustav se sastoji od dva Arduino Nano mikroupravljača koji međusobno komuniciraju putem I2C
sabirnice. Prvi Arduino Nano (MASTER) povezan je s ultrazvučnim senzorom HC-SR04 te služi za
mjerenje udaljenosti između vozila i prepreke. Izmjerena udaljenost pretvara se u numeričku
vrijednost i šalje drugom Arduinu (SLAVE). Drugi Arduino Nano prikazuje zaprimljene podatke na
LCD zaslonu te upravlja piezo buzzerom koji generira zvučna upozorenja ovisno o udaljenosti.
Logika ponašanja sustava
Logika sustava temelji se na kontinuiranom mjerenju udaljenosti. Ako je udaljenost manja od
definirane granice, sustav povećava učestalost zvučnog signala. Kako se vozilo približava
prepreci, interval između zvučnih upozorenja se smanjuje, čime se korisniku jasno signalizira
opasnost od sudara.
Tehničke specifikacije
Hardver
1. Arduino Nano (2 komada)
Arduino Nano je mikrokontrolerska razvojna pločica temeljena na mikrokontroleru ATmega328P.
Koristi se za upravljanje senzorima, obradbu podataka i upravljanje izlaznim uređajima.
Ima digitalne i analogne pinove, USB priključak za programiranje te radi na 5 V, što ga čini pogodnim za rad s većinom edukativnih modula.
________________________________________
2. Breadboard (eksperimentalna pločica)
Breadboard omogućuje izradu elektroničkih sklopova bez lemljenja.
Unutarnje vodljive staze povezuju redove rupa, što omogućuje brzo testiranje i izmjene spojeva.
Najčešće se koristi u edukaciji i prototipiranju.
________________________________________
3. Ultrazvučni senzor HC-SR04
HC-SR04 je senzor za mjerenje udaljenosti koji radi na principu emitiranja i prijema ultrazvučnih valova.
Vrijeme povratka vala koristi se za izračun udaljenosti objekta, najčešće u rasponu od 2 cm do 4 m.
Često se koristi u robotici i sustavima za detekciju prepreka.
________________________________________
4. LCD zaslon 16×2 (HD44780 kompatibilan)
LCD 16×2 zaslon može prikazati 16 znakova u dva reda.
Koristi se za prikaz teksta, brojeva i jednostavnih simbola.
Upravljanje se vrši paralelnom komunikacijom ili pomoću biblioteka u Arduinu.
________________________________________
5. Buzzer modul (aktivni, “High level trigger”)
Buzzer je elektroakustična komponenta koja pretvara električni signal u zvuk.
Aktivni buzzer proizvodi zvuk čim dobije napon, bez potrebe za PWM signalom.
Često se koristi za zvučna upozorenja i signalizaciju.
________________________________________
6. Otpornik
Otpornik ograničava protok električne struje u strujnom krugu i štiti osjetljive komponente od prevelike struje.
Vrijednost otpora određuje se prema potrebama sklopa i označena je obojenim prstenovima.
________________________________________
7. Spojne žice (jumper wires)
Spojne žice služe za povezivanje komponenti na breadboardu i s Arduinom.
Omogućuju fleksibilno slaganje sklopa i lako mijenjanje spojeva bez trajnih veza.
________________________________________
8. USB kabel
USB kabel se koristi za napajanje Arduino Nano pločice i za prijenos programa s računala na mikrokontroler.
Također omogućuje serijsku komunikaciju za praćenje rada sustava.

Programski kod sustava
Sav programski kod je pisan u Arduino IDE razvojnom okruženju, 
koje pruža podršku za programski jezik C/C++,
alate za kompajliranje i učitavanje programa na mikroupravljač te serijski monitor za praćenje rada sustava i ispis dijagnostičkih informacija.
Kod za MASTER: 
#include <Wire.h>

/* Definicija pinova i varijabli */
int trig=8;
int echo=10;
long vrijemeOcitavanja;
float udaljenostUCm=0;

/* Inicijalizacija serijske komunikacije, pinova i I2C sabirnice */
void setup()
{
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Wire.begin(); 
}

/* Glavna petlja programa */
void loop()
{
  /* Generiranje ultrazvučnog impulsa i mjerenje vremena povratka signala */
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);

  vrijemeOcitavanja=pulseIn(echo, 1, 30000);

  /* Izračun udaljenosti na temelju izmjerenog vremena */
  udaljenostUCm=RacunajUdaljenost(vrijemeOcitavanja);
  udaljenostUCm*=100;

  /* Provjera ispravnosti očitanja */
  if (vrijemeOcitavanja == 0) return;

  Serial.println(udaljenostUCm);

  /* Slanje udaljenosti SLAVE Arduinu putem I2C komunikacije */
  PosaljiUdaljenost(udaljenostUCm);

  delay(60);
}

/* Funkcija za izračun udaljenosti pomoću brzine zvuka */
float RacunajUdaljenost(long vrijeme)
{
  return (float)vrijeme*340.0/1000000.0/2.0;
}

/* Funkcija za slanje udaljenosti putem I2C sabirnice */
void PosaljiUdaljenost(float udaljenost)
{
  Wire.beginTransmission(8);
  Wire.write((byte*)&udaljenost, sizeof(udaljenost));
  Wire.endTransmission();
}
KOD ZA SLAVE 
// C++ code
// SLAVE

#include <LiquidCrystal.h>
#include <Wire.h>

/* Definicija pinova, LCD-a i varijabli */
int piezo=6;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float udaljenostUCm=0;
bool dosaoPodatak=false;

/* Inicijalizacija LCD-a, I2C komunikacije i piezo buzzera */
void setup()
{
  pinMode(piezo, OUTPUT);
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(primiUdaljenost);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Duljina: ");
}

/* Glavna petlja programa */
void loop()
{
  /* Provjera dolaska novog podatka i osvježavanje LCD reda */
  lcd.setCursor(0,1);
  if(dosaoPodatak)
  {
    obrisiRed(1);
    dosaoPodatak=false;
  }

  Serial.println(udaljenostUCm);

  /* Ispis udaljenosti na LCD zaslon */
  lcd.print((int)udaljenostUCm);
  lcd.print("cm");

  /* Zvučno upozorenje pomoću piezo buzzera */
  OglasiPiezo(udaljenostUCm);
}

/* I2C funkcija za primitak udaljenosti od MASTER Arduina */
void primiUdaljenost(int brBytova)
{
  if(brBytova==sizeof(udaljenostUCm))
  {
    Wire.readBytes((byte*)&udaljenostUCm, sizeof(udaljenostUCm));
    dosaoPodatak=true;
  }
}

/* Brisanje odabranog reda LCD zaslona */
void obrisiRed(int red)
{
  lcd.setCursor(0, red);
  lcd.print("                ");
}

/* Upravljanje piezo buzzerom ovisno o udaljenosti */
void OglasiPiezo(float udaljenost)
{
  static unsigned long zadnjiCiklus = 0;
  static bool svira = false;

  unsigned long sada = millis();
  unsigned long period;
  int freq = 500;
  const int trajanje = 50;

  if (udaljenost <= 30)
    period = 500;
  else if (udaljenost <= 70)
    period = 1000;
  else if (udaljenost <= 100)
    period = 3000;
  else
  {
    noTone(piezo);
    svira = false;
    return;
  }

  /* Pokretanje zvučnog signala */
  if (!svira && sada - zadnjiCiklus >= period)
  {
    tone(piezo, freq);
    svira = true;
    zadnjiCiklus = sada;
  }

  /* Zaustavljanje zvučnog signala */
  if (svira && sada - zadnjiCiklus >= trajanje)
  {
    noTone(piezo);
    svira = false;
  }
}

