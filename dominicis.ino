
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#include <TimeLib.h>
#include <DS1307RTC.h> 
#define ZONE_UTC +1

#include <decodeurGPZDA.h>
decodeurGPZDA gpzda;
#include <SoftwareSerial.h>
SoftwareSerial gps(2, 3);                              // RX, TX

#include <yasm.h>
#include <btn.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleDHT.h>

#define pinDHTint 7
#define pinDHText 6
#define pinDHTvmc 5
#define pinDSchau 4
#define pinRLalim 8
#define pinRLsens 9
#define ON HIGH
#define OFF LOW
#define OUVRE HIGH
#define FERME LOW


#define DHint 0
#define DHext 1
#define DHvmc 2
#define Tchau 3

#define ERREUR -127
#define ETE true
#define HIVER false

int P_periode_mesures = 5; //5secondes
int P_periode_enregistrement = 600; //600s = 10 min
int8_t P_hysteresis = 2;    //2°C
int8_t P_seuilEte = 19;
uint16_t P_tempoLCD = 600;
uint8_t P_tempoMvmt = 120;

float T[4], H[3];
int8_t tab6Text[6], tab24Text[24];
float Tmoy1int,  Tmoy1ext,  Tmoy24ext;

OneWire busDSChau(pinDSchau);
DallasTemperature dsChau(&busDSChau);

const int8_t pinDHT[3] = {pinDHTint, pinDHText, pinDHTvmc};
SimpleDHT22 dht;

#include <yasm.h>
#include <btn.h>
YASM mesures, menu, regul, retro, datalog;

bool saison = ETE;



void setup()
{
  
  Serial.begin(115200);
  gps.begin(4800);
  
  pinMode(pinRLalim, OUTPUT);
  pinMode(pinRLsens, OUTPUT);
  
  delay(1000);
  lcd.begin(16, 2); // set up the LCD's number of columns and rows:
  
  setSyncInterval(1000);
  setSyncProvider(RTC.get);    // the function to get the time from the RTC
  
  dsChau.begin(); // demarrage du capteur DS18B20
  dsChau.setWaitForConversion(false); // makes it async

  mesures.next(mesures_attente);
  menu.next(menu_heure);
  retro.next(retro_on);
  datalog.next(datalog_start);
  regul.next(regul_fermeture);
  


}

void loop()
{
  if (gps.available()) {
    if(gpzda.traiterCar(gps.read())) 
    {
      setTime(gpzda.heureUTC(), gpzda.minuteUTC(), gpzda.secondeUTC(), gpzda.jourUTC(), gpzda.moisUTC(), gpzda.anneeUTC());
      adjustTime(ZONE_UTC * SECS_PER_HOUR);    // ici on est à UTC + 1
      RTC.set(now());
    }
  }
  //retro.run();
  mesures.run();
  menu.run();
  //regul.run();
  //datalog.run();
}



