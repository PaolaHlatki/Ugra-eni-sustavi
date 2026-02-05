#include <Wire.h>

int trig=8;
int echo=10;
long vrijemeOcitavanja;
float udaljenostUCm=0;

void setup()
{
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Wire.begin(); 
}

void loop()
{
  digitalWrite(trig, 0);
  delayMicroseconds(2);
  digitalWrite(trig, 1);
  delayMicroseconds(10);
  digitalWrite(trig, 0);

  vrijemeOcitavanja=pulseIn(echo, 1, 30000);
  udaljenostUCm=RacunajUdaljenost(vrijemeOcitavanja);
  udaljenostUCm*=100;

  if (vrijemeOcitavanja == 0) return;

  Serial.println(udaljenostUCm);
  PosaljiUdaljenost(udaljenostUCm);
  delay(60);
}

float RacunajUdaljenost(long vrijeme)
{
  return (float)vrijeme*340.0/1000000.0/2.0;
}

void PosaljiUdaljenost(float udaljenost)
{
  Wire.beginTransmission(8);
  Wire.write((byte*)&udaljenost, sizeof(udaljenost));
  Wire.endTransmission();
}
