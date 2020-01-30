void regul_ouvert()
{
  if( ((mode==ETE)   && ((T[DHext]-P_hysteresis)>T[DHint])) || 
      ((mode==HIVER) && ((T[DHext]+P_hysteresis)<T[DHint])) )
    regul.next(regul_fermeture);
}

void regul_ferme()
{
  if( ((mode==ETE)   && (T[DHext]<T[DHint])) || 
      ((mode==HIVER) && (T[DHext]>T[DHint])) )
    regul.next(regul_ouverture);
}

void regul_fermeture()
{
  if (regul.isFirstRun()) mouvement(FERME);
  
  if (regul.elapsed(P_tempoMvmt*1000UL)) {
    off();
    regul.next(regul_ferme);
  }
}

void regul_ouverture()
{
  if (regul.isFirstRun()) mouvement(OUVRE);
  
  if (regul.elapsed(P_tempoMvmt*1000UL)) {
    off();
    regul.next(regul_ouvert);
  }
}

void mouvement(int8_t sens)
{
  digitalWrite(pinRLsens, sens);
  digitalWrite(pinRLalim, ON);
}

void off()
{
  digitalWrite(pinRLsens, OFF);
  digitalWrite(pinRLalim, OFF); 
}
