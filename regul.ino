void regul_registres_ouverts()
{
  if( ((saison==ETE) && ((T[DHext]-P_hysteresis)>T[DHint])) || 
    ((saison==HIVER) && ((T[DHext]+P_hysteresis)<T[DHint])) )
    regul.next(regul_fermeture);
}

void regul_registres_fermes()
{
  if( ((saison==ETE) && (T[DHext]<T[DHint])) || 
    ((saison==HIVER) && (T[DHext]>T[DHint])) )
    regul.next(regul_ouverture);
}

void regul_fermeture()
{
  if (regul.isFirstRun()) mouvement(FERME);
  
  if (regul.elapsed(P_tempoMvmt*1000UL)) {
    off();
    regul.next(regul_registres_fermes);
  }
}

void regul_ouverture()
{
  if (regul.isFirstRun()) mouvement(OUVRE);
  
  if (regul.elapsed(P_tempoMvmt*1000UL)) {
    off();
    regul.next(regul_registres_ouverts);
  }
}

void mouvement(int sens)
{
  digitalWrite(pinRLsens, sens);
  digitalWrite(pinRLalim, ON);
}

void off()
{
  digitalWrite(pinRLsens, OFF);
  digitalWrite(pinRLalim, OFF); 
}
