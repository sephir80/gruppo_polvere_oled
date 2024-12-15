#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define NELEMENTI_MAX 10
#define DELTA_TIME  5000
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_ADS1115 ads;

int16_t adc0 = 0;
float volt;
int16_t pesoinGrammiNew;
int16_t pesoinGrammiOld;
int16_t grammiAlMinuto;
int16_t indexElemento;
int16_t listaDeltaPeso[NELEMENTI_MAX];


void setup()
{
    Serial.begin(9600);
    ads.setGain(GAIN_TWOTHIRDS);
    ads.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 ALLOCATION ERROR"));
        for (;;);

    }
    display.display();
    delay(2000);
    adc0 = ads.readADC_SingleEnded(0);
    volt = tensione(adc0);
    pesoinGrammiOld = convertiInPeso(volt, 3000);
    indexElemento = 0;
}

void loop() {
    int16_t deltaPeso;

    adc0 = ads.readADC_SingleEnded(0);
    volt = tensione(adc0);
    pesoinGrammiNew = convertiInPeso(volt, 3000);

    grammiAlMinuto = calcolaGrammiAlMinuto(pesoinGrammiNew,pesoinGrammiOld);
    Serial.print("Analog input pin 0 : ");
    Serial.print(adc0);
    Serial.print(" volt : ");
    Serial.println(volt);

    Display3elementi(volt, grammiAlMinuto, pesoinGrammiNew);

    delay(DELTA_TIME);
}

float tensione(int16_t campione)
{
    float v;
    v = abs(campione * 0.0001875);
    return v;
}

float convertiInPeso(float volt, int GrammixVolt)
{
    int16_t g = 0;
    g = volt * GrammixVolt;
    return g;
}

int calcolaGrammiAlMinuto(int16_t pesoNew, int16_t pesoOld)
{
    int16_t gmin = 0;
    int16_t delta = 0;
    delta = pesoNew - pesoOld;
    listaDeltaPeso[indexElemento]=delta;
    indexElemento++;
    if (indexElemento==NELEMENTI_MAX)
    {
        for (auto d:listaDeltaPeso)
        {
            gmin += d;
        }
        gmin *= (60000/(NELEMENTI_MAX*DELTA_TIME));
        indexElemento = 0;
    }
    return gmin;
}

void Display3elementi(float v, int16_t grmin, int16_t grammi)
{
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("VOLT");
    display.setCursor(0, 16);
    display.print(v, 3);

    display.drawLine(64, 0, 64, 31, WHITE);

    display.setCursor(68, 0);
    display.print("GR/M");
    display.setCursor(88, 16);
    display.print(grmin);

    display.setCursor(25, 35);
    display.print("Peso");
    display.setTextSize(1);
    display.setCursor(78, 40);
    display.print("(grammi)");
    display.setTextSize(2);
    display.setCursor(30, 50);
    display.print(grammi);
    display.drawLine(0, 32, 128, 32, WHITE);
    display.display();
}