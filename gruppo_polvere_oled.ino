#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define LED_OVERFLOW 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define NELEMENTI_MAX 4
#define DELTA_TIME  5000
#define GRAMMI_PER_VOLT 1000
#define RISOLUZIONE_ADC 0.0001875
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
    pinMode(LED_OVERFLOW, OUTPUT);
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
    volt = tensione(adc0,RISOLUZIONE_ADC);
    pesoinGrammiOld = convertiInPeso(volt, GRAMMI_PER_VOLT);
  //  pesoinGrammiOld = 0;
    indexElemento = 0;
    grammiAlMinuto = 0;
}

void loop() {
    int16_t deltaPeso;

    adc0 = ads.readADC_SingleEnded(0);
    volt = tensione(adc0,RISOLUZIONE_ADC);
    if (volt>5.0 )
    {
        digitalWrite(LED_OVERFLOW, HIGH);
    }
    else
    {
        digitalWrite(LED_OVERFLOW, LOW);
    }
    pesoinGrammiNew = convertiInPeso(volt, GRAMMI_PER_VOLT);
  //  pesoinGrammiNew += 5;
    grammiAlMinuto = calcolaGrammiAlMinuto(pesoinGrammiNew,pesoinGrammiOld,grammiAlMinuto);

    StampaSeriale();
    Display3elementi(volt, grammiAlMinuto, pesoinGrammiNew);

    delay(DELTA_TIME);
    pesoinGrammiOld = pesoinGrammiNew;
}

void StampaSeriale()
{
    Serial.print(millis());
    Serial.print(";");
    Serial.print(pesoinGrammiNew);
    Serial.print(";");
    Serial.println(grammiAlMinuto);

/*    Serial.print("Analog input pin 0 : ");
    Serial.print(adc0);
    Serial.print(" volt : ");
    */
}

float tensione(int16_t campione,float adcResolution)
{
    float v;
    v = abs(campione * adcResolution);
    return v;
}

float convertiInPeso(float volt, int GrammixVolt)
{
    int16_t g = 0;
    g = (volt * GrammixVolt)-20;
    return g;
}

int calcolaGrammiAlMinuto(int16_t pesoNew, int16_t pesoOld, int16_t grammiOld)
{
    int16_t delta = 0;
    delta = pesoNew - pesoOld;
    listaDeltaPeso[indexElemento]=delta;
    indexElemento++;
    if (indexElemento==NELEMENTI_MAX)
    {
        grammiOld = 0;
        for (auto d:listaDeltaPeso)
        {
            grammiOld += d;
        }
        grammiOld *= (60000/(NELEMENTI_MAX*DELTA_TIME));
        indexElemento = 0;
    }
    return grammiOld;
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
    display.print(abs( grmin));

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