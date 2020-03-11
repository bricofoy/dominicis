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

void fatDateTime(uint16_t* date, uint16_t* time) {
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(year(), month(), day());
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(hour(), minute(), second());
}

/////////////////////////datalog state machine//////////////////////////////////
void datalog_setup()
{
  // set date time callback function
  SdFile::dateTimeCallback(fatDateTime);
  
  datalog.next(datalog_start);
}


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
      lcd.setCursor(14,0);
      lcd<<F("er");
    }
  }
  
  if(SdOK) datalog.next(datalog_write);				
}



void datalog_write()
{
  if(datalog.isFirstRun()) {
    FileOK=Logfile.open(nomDuFichierLog(), O_RDWR | O_CREAT | O_AT_END);
    if(!FileOK)
    {
      SdOK=false;
      datalog.next(datalog_start);    
      return;
    }
    //temoin d'activité de la carte sur l'écran
    lcd.setCursor(14,0);
    lcd<<F("ec");
    
    //si le fichier est nouveau, on ajoute les étiquettes sur la première ligne
    if(Logfile.fileSize()<10) Logfile<<F("Date;Tint;Hint;Text;Hext;Tvmc;Hvmc;Tchau;Text moy 10m;Text moy 24h;Registre F/O;Saison H/E;Mode auto/O/F")<<_endl;
    
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
    Logfile<<saison<<F(";")<<mode;
    //fin de ligne et fermeture du fichier
    Logfile<<_endl;
    Logfile.close();
    
    //on efface le témoin d'activité
    lcd.setCursor(14,0);
    lcd<<F("  ");
    
    datalog.next(datalog_wait);
  }
}
