
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();



#include <yasm.h>
#include <btn.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleDHT.h>

#define pinDHTint 7
#define pinDHText 6
#define pinDHTvmc 5
#define pinDSchau 4


#define DHint 0
#define DHext 1
#define DHvmc 2
#define Tchau 3

#define ERREUR 666

int P_periode_mesures = 5; //5secondes
int P_periode_enregistrement = 600; //600s = 10 min
int8_t P_hysteresis = 2;    //2°C
uint8_t P_tempoLCD = 60;

float T[4], H[3];

OneWire busDSChau(pinDSchau);
DallasTemperature dsChau(&busDSChau);

const int8_t pinDHT[3] = {pinDHTint, pinDHText, pinDHTvmc};
SimpleDHT22 dht;

#include <yasm.h>
#include <btn.h>
YASM mesures, menu, regul, retro, datalog;

bool Hiver = false;



void setup()
{
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  delay(1000);
  lcd.begin(16, 2);

  mesures.next(mesures_attente);
  menu.next(menu_debut);
  retro.next(retro_on);
  datalog.next(datalog_start);
  regul.next(regul_attente);
  


}

void loop()
{
  retro.run();
  mesures.run();
  menu.run();
  regul.run();
  datalog.run();
}



