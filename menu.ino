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
}

void menu_afficheT()
{
  uint8_t i = menu.runCount();

  if (menu.isFirstRun()) {
    //lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(15,0);
    lcd.print(SdOK?" ":"E");
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

void retro_on()
{
  if (retro.isFirstRun()) {
    //lcd.begin(16, 2);
    lcd.setBacklight(0x01);
  }

  if (retro.elapsed(P_tempoLCD * 1000UL)) retro.next(retro_off);
}

void retro_off()
{
  if (retro.isFirstRun()) {
    lcd.setBacklight(0);
  }

  if (lcd.readButtons()) retro.next(retro_on);
}
