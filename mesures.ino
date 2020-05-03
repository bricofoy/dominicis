#include <math.h>

struct info_t {
  bool init6 : 1;
  uint8_t index6 : 3;
  bool init24 : 1;
  uint8_t index24 : 5;
} info;

void mesures_attente()
{
  if (mesures.elapsed(P_periode_mesures*1000UL - DELAI_MAJ_DHT)) 
    mesures.next(mesures_purge);
}

void mesures_purge()
{
  uint8_t i = mesures.runCount();
  
  if (i < sizeof(pinDHT) ) 
    dht.read(pinDHT[i], NULL, NULL, NULL); //lecture juste pour renouveler la valeur
    else {
      mesures.next(mesures_attenteAquisition);
      dsChau.requestTemperatures();
    }
}

void mesures_attenteAquisition()
{
  if (mesures.elapsed(DELAI_MAJ_DHT))
    mesures.next(mesures_lectureCapteurs);
}

void mesures_lectureCapteurs()
{
  uint8_t i = mesures.runCount();
  
  if (i < sizeof(pinDHT) ) {
    if (dht.read2(pinDHT[i], &T[i], &H[i], NULL) != SimpleDHTErrSuccess) T[i] = H[i] = ERREUR;
  }
  else {
    T[i]=dsChau.getTempCByIndex(0);
    mesures.next(mesures_majMoyennes);
  }
}

void mesures_majMoyennes()
{
  static time_t  derniereFois = 0;
  if((now()-derniereFois) < (10 * 60)) {
    mesures.next(mesures_attente);
    return; //on ne met à jour les moyennes que toutes les 10 minutes
  }
  
  if(T[DHext]==ERREUR) {
    mesures.next(mesures_purge);//on a besoin d'une valeur correcte pour maj les moyennes
    return;
  }
  
  tab6Text[info.index6]=round(T[DHext]*10); //*10 pour garder une décimale alors que c'est un tableau de int pour économiser de la RAM
  Tmoy6ext=0;
  for(uint8_t i=0;i<6;i++) Tmoy6ext+=tab6Text[i]; //on somme les éléments du tableau
  Tmoy6ext/=(float)info.init6?6:(info.index6+1); //puis division par le nombre d'éléments valides dans le tableau uniquement
  Tmoy6ext/=10; //et on /10 pour récupérer la décimale dans la valeur finale
  
  if(++info.index6==6){
    info.init6=true;
    info.index6=0;
  }
  
  if(info.index6==1) { //==1 et non 0 pour ne commencer à recopier dans le tableau des 24 éléments que lorsqu'il y a au moins une valeur dans celui de 6, ça évite de démarrer la moyenne avec un zéro
    tab24Text[info.index24]=round(Tmoy6ext*10);
    Tmoy24ext=0;
    for(uint8_t i=0;i<24;i++) Tmoy24ext+=tab24Text[i];
    Tmoy24ext/=(float)info.init24?24:(info.index24+1);
    Tmoy24ext/=10;
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
