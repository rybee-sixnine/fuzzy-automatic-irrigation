#include <LiquidCrystal.h>
#include <DHT.h>
#include <Servo.h>

#define DHTPIN 8
#define DHTTYPE DHT22

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
DHT dht(DHTPIN, DHTTYPE);
Servo indikator;

const int RED = 9;
const int GREEN = 10;
const int BLUE = 11;
const int soilPin = A0;

float suhu;
int soilValue;

float basah, lembab, kering;
float dingin, normal, panas;

void setColor(int r, int g, int b) {
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}

void setup() {
  lcd.begin(16, 2);
  dht.begin();

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  indikator.attach(12);
}

void loop() {

  soilValue = analogRead(soilPin);
  suhu = dht.readTemperature();
  if (isnan(suhu)) return;

  // fuzzy soil
  basah = 0;
  lembab = 0;
  kering = 0;

  if (soilValue <= 300) basah = 1;
  else if (soilValue > 300 && soilValue < 500)
    basah = (500 - soilValue) / 200.0;

  if (soilValue >= 300 && soilValue <= 500)
    lembab = (soilValue - 300) / 200.0;
  else if (soilValue > 500 && soilValue <= 700)
    lembab = (700 - soilValue) / 200.0;

  if (soilValue >= 700) kering = 1;
  else if (soilValue > 500 && soilValue < 700)
    kering = (soilValue - 500) / 200.0;

  // fuzzy suhu
  dingin = 0;
  normal = 0;
  panas = 0;

  if (suhu <= 22) dingin = 1;
  else if (suhu > 22 && suhu < 26)
    dingin = (26 - suhu) / 4.0;

  if (suhu >= 22 && suhu <= 26)
    normal = (suhu - 22) / 4.0;
  else if (suhu > 26 && suhu <= 30)
    normal = (30 - suhu) / 4.0;

  if (suhu >= 30) panas = 1;
  else if (suhu > 26 && suhu < 30)
    panas = (suhu - 26) / 4.0;

  float r1 = min(kering, panas);
  float r2 = min(kering, normal);
  float r3 = min(kering, dingin);
  float r4 = min(lembab, panas);
  float r5 = min(lembab, normal);
  float r6 = basah;

  float z =
    (r1 * 140 +
     r2 * 90 +
     r3 * 45 +
     r4 * 90 +
     r5 * 45 +
     r6 * 0)
    /
    (r1 + r2 + r3 + r4 + r5 + r6);

  indikator.write(z);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("S:");
  lcd.print(soilValue);
  lcd.print(" T:");
  lcd.print((int)suhu);

  lcd.setCursor(0,1);

  if(z > 110){
    lcd.print("BANYAK");
    setColor(255,0,0);
  }
  else if(z > 60){
    lcd.print("SEDANG");
    setColor(0,255,0);
  }
  else if(z > 20){
    lcd.print("SEDIKIT");
    setColor(255,255,0);
  }
  else{
    lcd.print("TUTUP");
    setColor(0,0,255);
  }

  delay(500);
}