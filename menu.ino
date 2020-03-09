#include <btn.h>

BTN gauche,droite,haut,bas,select;

void printValue(float x)
{
  if (x == ERREUR) {
    lcd.print("--.-");
    return;
  }
  
  if (x >= 100) {
    lcd<<" ";
    lcd.print(x, 0);
    return;
  }
  
  if (x >= 0 && x < 10) lcd.print(" ");
  
  lcd.print(x, 1);
}

void printTime()
{
  if(hour()<10) lcd<<" ";
  lcd<<hour()<<":";
  if(minute()<10) lcd<<"0";
  lcd<<minute();
}

// 0132456789012345
// JJ/MM/AAAA HH:MM 
// ETE AUTO OUV  OK



void menu_debut()
{
  if(menu.isFirstRun()){
    lcd.clear();
  }
  
  if(menu.periodic(1000)) {
    lcd.setCursor(0,0);
    printTime();
    
  }
}

void menu_afficheT()
{
  uint8_t i = menu.runCount();
  
  if (menu.isFirstRun()) {
    //lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(15,0);
    lcd.print((SdOK&&FileOK)?" ":"E");
  }
  
  if (i <= sizeof(pinDHT-1)) {
    lcd.setCursor( i * 5, 0);
    printValue(T[i]);
  }
  else menu.next(menu_afficheH);
}

void menu_afficheH()
{
  uint8_t i = menu.runCount();
  
  if (i <= (sizeof(pinDHT) - 1)) {
    lcd.setCursor( i * 5, 1);
    printValue(H[i]);
  }
  else menu.next(menu_debut);
}


void menu_heure()
{
  if(menu.periodic(800))
  {
    //lcd.clear();
    lcd.setCursor(0,0);
    printTime();
    lcd<<" ";
    printValue(Tmoy6ext);
    lcd<<" ";
    printValue(Tmoy24ext);
    
    lcd.setCursor(0,1);
    lcd<<(saison==ETE?F("ETE"):F("HIV"));
    lcd<<((regul.isInState(regul_registres_ouverts) || regul.isInState(regul_ouverture))?F(" O "):F(" F "));
    printValue(T[DHint]);
    lcd<<" ";
    printValue(T[DHext]);
  }
  
  if (select.state(BTN_CLICK)) {lcd<<Tmoy6ext; regul.next(regul_fermeture);}
  if (select.state(BTN_LONGCLICK)) regul.next(regul_ouverture);
}


void retro_on()
{
  if (retro.isFirstRun()) {
    //lcd.begin(16, 2);
    lcd.setBacklight(0x01);
  }
  
  if (retro.elapsed(P_tempoLCD * 1000UL)) retro.next(retro_off);
  
  //lecture des boutons
  uint8_t buttons = lcd.readButtons();
  haut.update(buttons & BUTTON_UP);
  bas.update(buttons & BUTTON_DOWN);
  gauche.update(buttons & BUTTON_LEFT);
  droite.update(buttons & BUTTON_RIGHT);
  select.update(buttons & BUTTON_SELECT);
}

void retro_off()
{
  if (retro.isFirstRun()) {
    lcd.setBacklight(0);
  }
  
  if (lcd.readButtons()) retro.next(retro_on);
}
