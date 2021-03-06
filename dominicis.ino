

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
SoftwareSerial gps(2, 3);   // RX, TX

#include <Streaming.h>

#include <yasm.h>
#include <btn.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SimpleDHT.h>
#define DELAI_MAJ_DHT 2600UL

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

#define AUTO 0
#define OUVERT 1
#define CLOSED 2

int P_periode_mesures = 5;            //5secondes
int P_periode_enregistrement = 300;   //300s = 5 min
int8_t P_hysteresis = 2;              //2°C
int8_t P_seuilEte = 19;
uint16_t P_tempoLCD = 120;
uint8_t P_tempoMvmt = 155;            //2m 35s

float T[4], H[3];
int tab6Text[6], tab24Text[24];
float Tmoy6ext,  Tmoy24ext;

OneWire busDSChau(pinDSchau);
DallasTemperature dsChau(&busDSChau);

const int8_t pinDHT[3] = {pinDHTint, pinDHText, pinDHTvmc};
SimpleDHT22 dht;

#include <yasm.h>
#include <btn.h>
YASM mesures, menu, regul, retro, datalog;

bool saison = ETE;
uint8_t mode = AUTO;



void setup()
{

  Serial.begin(115200);
  Serial<<"init"<<_endl;
  gps.begin(9600);

  pinMode(pinRLalim, OUTPUT);
  pinMode(pinRLsens, OUTPUT);
  off();

  lcd.begin(16, 2);                      // set up the LCD's number of columns and rows
  
  for(uint8_t i=0;i<(sizeof(T)/sizeof(T[0]));i++) {asm("nop"); T[i]=ERREUR; } 
  for(uint8_t i=0;i<(sizeof(H)/sizeof(H[0]));i++) {asm("nop"); H[i]=ERREUR; }
  
  setSyncProvider(RTC.get);              // the function to get the time from the RTC

  dsChau.begin();                        // demarrage du capteur DS18B20
  dsChau.setWaitForConversion(false);    // makes it async

  mesures.next(mesures_purge);
  menu.next(menu_heure);
  retro.next(retro_on);
  datalog.next(datalog_setup);
  regul.next(regul_fermeture);



}

void loop()
{
  while (gps.available()) {
    if(gpzda.traiterCar(gps.read())) 
    {
      time_t before = now(); //gardons en mémoire la valeur de l'horloge avant la lecture gps
      
      setSyncProvider(NULL); //ça c'est juste pour être sûr de ne pas re-synchroniser avant la fin du test
      
      setTime(gpzda.heureUTC(), gpzda.minuteUTC(), gpzda.secondeUTC(), gpzda.jourUTC(), gpzda.moisUTC(), gpzda.anneeUTC()); //lecture du temps GPS (temps UTC)
      adjustTime(ZONE_UTC * SECS_PER_HOUR);   // ici on est à UTC + 1 (ZONE_UTC = 1)
      
      // calcul de la valeur absolue du décale entre le temps gps et l'horloge RTC
      if (before > now()) 
        before = before - now();
      else 
        before = now()-before;
      
      // si l'heure gps diffère de plus ou moins 30s de l'heure RTC,  on synchronise la RTC,  
      if (before >30) 
              RTC.set(now());
      
      //sinon on la relit.
      setSyncProvider(RTC.get);
      
      setSyncInterval(300);            // retour à la valeur par défaut
    } 
  }
  
  //execution des différentes tâches :
  retro.run();
  mesures.run();
  menu.run();
  regul.run();
  datalog.run();
}



