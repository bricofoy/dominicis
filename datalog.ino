#include <SPI.h>
#include <SdFat.h>
#include <Streaming.h>

#define PIN_SD_CS	SS

SdFat Sd; // File system object.
SdFile Logfile; // Log file.

bool SdOK=false, FileOK=false; //some flags

char * nomDuFichierLog() 
{
  //AUTEUR J-M-L pour forum Arduino http://forum.arduino.cc/index.php?topic=559714.0
  
  // on crée un fichier par mois
  static char _nomDuFichierLog[13]; //  "AAAA-MM.CSV"
  
  // Convertir les données en chaines http://www.cplusplus.com/reference/cstdlib/itoa/
  itoa (year(), _nomDuFichierLog, 10); // initialise les 4 premiers caractères
  _nomDuFichierLog[4] = '-';
  
  if (month() < 10) { // on met un zéro
    _nomDuFichierLog[5] = '0';
    _nomDuFichierLog[6] = '0' + month();
  } else {
    _nomDuFichierLog[5] = '1';
    _nomDuFichierLog[6] = '0' + month() - 10;
  }
  
  _nomDuFichierLog[7] = '\0';
  strcat(_nomDuFichierLog, ".csv");
  return _nomDuFichierLog;
}

/////////////////////////datalog state machine//////////////////////////////////
void datalog_wait()
{
  if(datalog.elapsed(P_periode_enregistrement * 1000UL) )
    datalog.next(datalog_start);
}


void datalog_start()
{
  if(datalog.periodic(2000)) {
    SdOK=Sd.begin(PIN_SD_CS, SPI_HALF_SPEED);
    if(!SdOK) {
      lcd.setCursor(15,0);
      lcd<<F("x");
    }
  }
  
  if(SdOK) datalog.next(datalog_write);				
}



void datalog_write()
{
  if(datalog.isFirstRun()) {
    FileOK=Logfile.open(nomDuFichierLog(), O_RDWR | O_CREAT | O_AT_END);
    lcd.setCursor(15,0);
    if(!FileOK)
    {
      lcd<<F("x");
      SdOK=false;
      datalog.next(datalog_start);    
      return;
    }
    lcd<<F("w");
    
    //si le fichier est nouveau, on ajoute les étiquettes sur la première ligne
    if(Logfile.fileSize()<10) Logfile<<F("Date;Tint;Hint;Text;Hext;Tvmc;Hvmc;Tchau;Text moy 10m;Text moy 24h;Registre F/O;Mode H/E")<<_endl;
    
    //date et heure en début de ligne
    Logfile<<year()<<F("-")<<month()<<F("-")<<day()<<F(" ");
    Logfile<<hour()<<F(":")<<minute()<<F(":")<<second()<<F(";");
  }
  
  uint8_t i=datalog.runCount();
  
  //températures et humidités
  if(i<3) Logfile<<_FLOAT(T[i],1)<<F(";")<<_FLOAT(H[i],1)<<F(";");
  if(i==3) Logfile<<_FLOAT(T[i],1)<<F(";");
  
  if(i==4) {
    //moyennes Text
    Logfile<<_FLOAT(Tmoy6ext,1)<<F(";")<<_FLOAT(Tmoy24ext,1)<<F(";");
    //etat des registres
    Logfile<<(regul.isInState(regul_registres_ouverts) || regul.isInState(regul_ouverture))<<F(";");
    //mode
    Logfile<<saison;
    Logfile<<_endl;
    Logfile.close();
    lcd.setCursor(15,0);
    lcd<<F(" ");
    datalog.next(datalog_wait);
  }
}
