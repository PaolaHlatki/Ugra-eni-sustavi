//SLAVE KOD
#include <LiquidCrystal.h>
#include <Wire.h>

int piezo=6;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
float udaljenostUCm=0;
bool dosaoPodatak=false;

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

void loop()
{
  lcd.setCursor(0,1);
    if(dosaoPodatak)
    {
      obrisiRed(1);
      dosaoPodatak=false;
    }
  Serial.println(udaljenostUCm); 
  lcd.print((int)udaljenostUCm);
  lcd.print("cm");
  OglasiPiezo(udaljenostUCm);
}

void primiUdaljenost(int brBytova)
{
  if(brBytova==sizeof(udaljenostUCm))
  {
    Wire.readBytes((byte*)&udaljenostUCm, sizeof(udaljenostUCm));
    dosaoPodatak=true;
  }
}

void obrisiRed(int red)
{
  lcd.setCursor(0, red);
  lcd.print("                ");
}

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

  if (!svira && sada - zadnjiCiklus >= period)
  {
    tone(piezo, freq);
    svira = true;
    zadnjiCiklus = sada;
  }

  if (svira && sada - zadnjiCiklus >= trajanje)
  {
    noTone(piezo);
    svira = false;
  }
}
