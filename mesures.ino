#include <math.h>

void mesures_attente()
{
  if (mesures.elapsed(10*1000UL)) //10 secondes
    mesures.next(mesures_purge);
}

void mesures_purge()
{
  uint8_t i = mesures.runCount();
  
  if (i < sizeof(pinDHT) ) 
    dht.read(pinDHT[i], NULL, NULL, NULL); //lecture juste pour renouveller la valeur
    else {
      mesures.next(mesures_attenteAquisition);
      dsChau.requestTemperatures();
    }
    
}

void mesures_attenteAquisition()
{
  if (mesures.elapsed(2500))
    mesures.next(mesures_lectureCapteurs);
}

void mesures_lectureCapteurs()
{
  uint8_t i = mesures.runCount();
  
  if (i < sizeof(pinDHT) ) {
    if (dht.read2(pinDHT[i], &T[i], &H[i], NULL) != SimpleDHTErrSuccess) {
      //mesures.next(mesures_attenteAquisition);
      T[i] = H[i] = ERREUR;
    }
  }
  else {
    T[i]=dsChau.getTempCByIndex(0);
    mesures.next(mesures_majMoyennes);
  }
}

void mesures_majMoyennes()
{
  static struct {
    bool init6 : 1;
    uint8_t index6 : 3;
    bool init24 : 1;    
    uint8_t index24 : 5;
  } info={false,0,false,0};
  
  static time_t  derniereFois = 0;
  
  if((now()-derniereFois) < (10 * 60)) {
    mesures.next(mesures_attente);
    return; //on ne met à jour les moyennes que toutes les 10 minutes
  }
  
  tab6Text[info.index6]=round(T[DHext]);
  for(uint8_t i=0;i<6;i++) {
    Tmoy1ext+=tab6Text[i];
  }
  Tmoy1ext=Tmoy1ext/info.init6?6:info.index6;
  if(++info.index6==6){
    info.init6=true;
    info.index6=0;
  }
  
  if(info.index6=0) {
    tab24Text[info.index24]=Tmoy1ext;
    for(uint8_t i=0;i<24;i++) Tmoy24ext+=tab24Text[i];
    Tmoy24ext=Tmoy24ext/info.init24?24:info.index24;
    if(++info.index24==24){
      info.init24=true;
      info.index24=0;
    }
    if(Tmoy24ext>P_seuilEte) saison = ETE;
    else saison = HIVER;
  }
  derniereFois = now();
  mesures.next(mesures_attente);
}
