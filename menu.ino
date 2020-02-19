#include <btn.h>

BTN gauche,droite,haut,bas,select;

void printX(float x)
{
  if (x == ERREUR) {
    lcd.print("--.-");
    return;
  }
  
  if (x > 100) {
    lcd.print(x, 0);
    return;
  }
  
  if (x >= 0 && x < 10) lcd.print(" ");
  
  lcd.print(x, 1);
}



void menu_debut()
{
  if ( menu.elapsed(1500))
    menu.next(menu_afficheT);
  
  if(gauche.state(BTN_CLICK))
    menu.next(menu_heure);
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
    printX(T[i]);
  }
  else menu.next(menu_afficheH);
}

void menu_afficheH()
{
  uint8_t i = menu.runCount();
  
  if (i <= (sizeof(pinDHT) - 1)) {
    lcd.setCursor( i * 5, 1);
    printX(H[i]);
  }
  else menu.next(menu_debut);
}


void menu_heure()
{
  if(menu.periodic(800))
  {
    //lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(hour());
    lcd.print(":");
    lcd.print(minute());
    lcd.print(":");
    //lcd.print(second());
    //lcd.setCursor(0,1);
    lcd.print(now());
    
  }
  
  
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
