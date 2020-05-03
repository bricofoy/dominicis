#include <btn.h>

#define DEG (char)-33

BTN gauche,droite,haut,bas,select;

void print0() {lcd<<F("0");}
void print2p() {lcd<<F(":");}
void printSl() {lcd<<F("/");}
void printSp(uint8_t nb=1) 
{
  for(nb;nb>0;nb--) 
    lcd<<F(" ");
}


void printValue(float x)
{
  if (x == ERREUR) {
    lcd<<F("--.-");
    return;
  }
  if (x >= 100) {
    printSp();
    lcd.print(x, 0);
    return;
  }
  if (x >= 0 && x < 10) printSp();
  lcd.print(x, 1);
}

void printHeure(bool sec=false)
{
  if(hour()<10) print0();
  lcd<<hour(); print2p();
  if(minute()<10) print0();
  lcd<<minute(); 
  if(sec) {
    print2p();
    if(second()<10) print0();
    lcd<<second(); 
  }
}

void printDate()
{
  if(day()<10) print0();
  lcd<<day(); printSl();
  if(month()<10) print0();
  lcd<<month(); printSl();
  lcd<<year();
}

void printMode() 
{
  lcd<<(mode==AUTO?F("AUTO"):F("MANU"));
}

void printSaison()
{
  lcd<<(saison==ETE?F("ETE"):F("HIV"));
}

void printOF()
{
  lcd<<((regul.isInState(regul_registres_ouverts) || regul.isInState(regul_ouverture))?F("O"):F("F"));
}

void printDC() {lcd<<DEG<<F("C");}
void printRH() {lcd<<F("%HR");}

void purgeBtn() {
  haut.state();
  bas.state();
  droite.state();
  gauche.state();
  select.state();
}

////////////////////////////MENU/////////////////////////////////////////////////////

// 0132456789012345
// JJ/MM/AAAA SD:ok 
// HH:MM:SS AUTO F

void menu_heure()
{
  if(menu.isFirstRun()) {
    lcd.begin(16, 2); //au cas où l'i2c aurait buggé
    purgeBtn();
    lcd.clear();
  }
  
  if(menu.periodic(1000)) {
    //ligne 1
    lcd.setCursor(0,0);
    printDate(); printSp(); lcd<<F("SD"); print2p(); 
    if(SdOK && datalog.isInState(datalog_wait)) lcd<<F("ok");
    
    //ligne 2
    lcd.setCursor(0,1); 
    printHeure();
    printSp(); printMode(); printSp(); printSaison(); printSp(); printOF();
  }

  if(bas.state(BTN_CLICK)) menu.next(menu_ext);
  if(haut.state(BTN_CLICK)) menu.next(menu_moy);
  
  if(gauche.state(BTN_LONGCLICK)) menu.next(menu_etat);
  if(droite.state(BTN_LONGCLICK)) menu.next(menu_relais);
}

// 0132456789012345
// in--.- ext__._ 
// me--.- s-- ETE F
//       MANU 

void menu_etat()
{
  if(menu.isFirstRun()) {
    purgeBtn();
    lcd.clear();
  }
  
  if(menu.periodic(800)) {
    //ligne 1
    lcd.setCursor(0,0);
    lcd<<F("in"); printValue(T[DHint]); printSp();
    lcd<<F("ext"); printValue(T[DHext]);
    
    //ligne 2
    lcd.setCursor(0,1); 
    lcd<<F("me"); printValue(Tmoy24ext);
    if(mode==AUTO) {
      printSp();
      lcd<<F("s")<<P_seuilEte;
    }
    else printMode();
    printSp();
    printSaison();
    printSp(); printOF();
  }
  
  if(gauche.state(BTN_LONGCLICK)) menu.next(menu_mode);
  if(bas.state(BTN_CLICK)) menu.next(menu_ext);
  if(haut.state(BTN_CLICK)) menu.next(menu_heure);
}

// 0132456789012345
// AUTO ETE F
// xxx/155

void menu_relais()
{
  if(menu.isFirstRun()) {
    purgeBtn();
    lcd.clear();
  }
  
  if(menu.periodic(800)) {
    //ligne 1
    lcd.setCursor(0,0);
    printMode(); printSp(); printSaison(); printSp(); printOF();
    
    //ligne 2
    lcd.setCursor(0,1); 
    if(regul.isInState(regul_fermeture) || regul.isInState(regul_ouverture)) {
      lcd<<(regul.timeOnState()/1000)<<"/"<<P_tempoMvmt;
    }
  }
  
  if(gauche.state(BTN_CLICK)) menu.next(menu_heure);
  if(bas.state(BTN_CLICK)) menu.next(menu_ext);
  if(haut.state(BTN_CLICK)) menu.next(menu_heure);
}

// 0132456789012345
// Choix mode
//      AUTO F
//      MANU O
//      MANU F

void menu_mode()
{
  if(menu.isFirstRun()) {
    lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Choix mode");
  }
  
  if(menu.periodic(500)) {
    lcd.setCursor(4,1); printMode(); printSp(); printOF();
  }
  
  if(haut.state(BTN_CLICK)||bas.state(BTN_CLICK)) {
    mode++;
    if(mode>2) mode=AUTO;
    if(mode==OUVERT) regul.next(regul_ouverture);
    if(mode==CLOSED) regul.next(regul_fermeture);
  }
  
  if (menu.elapsed(P_tempoLCD * 1000UL)) menu.next(menu_heure);

  if(gauche.state(BTN_LONGCLICK)) menu.next(menu_heure);
}

// 0132456789012345
// Exterieur
// --.-°C --.-%HR
void menu_ext()
{
  if(menu.isFirstRun()) {
    lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Exterieur");
  }

  if(menu.periodic(1000)) {
    //ligne 2
    lcd.setCursor(0,1); printValue(T[DHext]); printDC(); printSp(); 
    printValue(H[DHext]); printRH();
  }
  
  if(haut.state(BTN_CLICK)) menu.next(menu_heure);
  if(bas.state(BTN_CLICK)) menu.next(menu_int);
}

// 0132456789012345
// Interieur
// --.-°C --.-%HR
void menu_int()
{
  if(menu.isFirstRun()) {
    //lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Interieur");
  }
  
  if(menu.periodic(1000)) {
    //ligne 2
    lcd.setCursor(0,1); printValue(T[DHint]); printDC(); printSp(); 
    printValue(H[DHint]); printRH();
  }
  
  if(haut.state(BTN_CLICK)) menu.next(menu_ext);
  if(bas.state(BTN_CLICK)) menu.next(menu_vmc);
}

// 0132456789012345
// Rejet VMC
// --.-°C --.-%HR
void menu_vmc()
{
  if(menu.isFirstRun()) {
    //lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Rejet VMC");
  }
  
  if(menu.periodic(1000)) {
    //ligne 2
    lcd.setCursor(0,1); printValue(T[DHvmc]); printDC(); printSp(); 
    printValue(H[DHvmc]); printRH();
  }
  
  if(haut.state(BTN_CLICK)) menu.next(menu_int);
  if(bas.state(BTN_CLICK)) menu.next(menu_chau);
}
  
// 0132456789012345
// Chauffage
// --.-°C
void menu_chau()
{
  if(menu.isFirstRun()) {
    lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Chauffage");
  }
  
  if(menu.periodic(1000)) {
    //ligne 2
    lcd.setCursor(0,1); printValue(T[Tchau]); printDC();
  }
  
  if(haut.state(BTN_CLICK)) menu.next(menu_vmc);
  if(bas.state(BTN_CLICK)) menu.next(menu_moy);
}

// 0132456789012345
// Moyennes Text
// 10m--.-  24h--.-
void menu_moy()
{
  if(menu.isFirstRun()) {
    lcd.clear();
    lcd.setCursor(0,0); lcd<<F("Moyennes Text");
  }

  if(menu.periodic(1000)) {
    //ligne 2
    lcd.setCursor(0,1); lcd<<F("10m"); printValue(Tmoy6ext); 
    printSp(); lcd<<F("24h"); printValue(Tmoy24ext);
  }
  
  if(haut.state(BTN_CLICK)) menu.next(menu_chau);
  if(bas.state(BTN_CLICK)) menu.next(menu_heure);
}


///////////////////////RETROECLAIRAGE///////////////////////////////////////////////////////////

void retro_on()
{
  if (retro.isFirstRun()) lcd.setBacklight(0x01);
  
  if (retro.elapsed(P_tempoLCD * 1000UL)) retro.next(retro_off);
  
  //lecture des boutons
  uint8_t buttons = lcd.readButtons();
  
  //si il y a eu de l'activité, raz du chrono
  if(buttons) retro.next(retro_on, true);
  
  //et on transmet l'activité à qui de droit
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
  
  if (lcd.readButtons()) {
    retro.next(retro_on);
    //delay(200);
    menu.next(menu_heure,true); //pour repartir toujours sur le même écran à l'allumage
  }
}
