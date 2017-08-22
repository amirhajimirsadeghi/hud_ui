#include <Adafruit_SSD1306.h>
#include "btnq.h"
#include "settings.h"
#include "divelog.h"
#include "next_dive.h"
#include "current_dive.h"

#define OLED_RESET 4

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
/* MEMORY: 
 * EEPROM 1-3 : Settings
 * EEPROM 4 : LOGBOOK COUNT
 * EEPROM 5-9,10-14,...,500-504: DIVE LOGS -- up to 100 dives
 * EEPROM 505-508: NEXT DIVE PLAN
*/
/* STATES:
 * 0: settings(view), 1: settings(edit_temp), 2: settings(edit_dist), 3: settings(edit_algo), 
 * 4: settings(edit_done), 5: divelog, 6: plan next dive, 7:set 02 level, 8: set depth level, 
 * 9: set surface time, 10: ascent mode, 11: ascent mode description, 12: descent mode, 
 * 13: descent mode description
*/
Adafruit_SSD1306 display(OLED_RESET);
int state = 0;

int nbts = 3;
int bts[3] = {2,4,7};
boolean btgs[3];
BtnQ* btnq;
int clicked;
//char toPrint[6] = {'\0','\0','\0','\0','\0','\0'};  //debugging button queue

int curr_dive_focus;

Settings* s;
DiveLog* d;
NextDive* nd;
CurrentDive* cd;

void setup() {
  Serial.begin(9600);
  
  //data stuff
  s = new Settings();
  d = new DiveLog();
  nd = new NextDive();
  cd = new CurrentDive();
  
  //d->addLog(23,40,60,20,8,17);
  curr_dive_focus = d->getCount();
  //button stuff
  for(int i=0; i<nbts; i++) btgs[i] = false;
  btnq = new BtnQ();
  
  //display stuff
  display.begin(0x3C);
  display.setTextColor(WHITE);
  display_settings();
  
}

void loop() {
  handle_buttons();
  clicked = btnq->pop();
  //react based on button clicked
  switch(state){
    case 0: { //view settings
      if(clicked == 1){
        state = 1;
        toggle_edit_done();
        toggle_temp_settings();
      }else if(clicked == 2){
        //go to dive logs
        state = 5;
        display_divelog(curr_dive_focus);
      }else{
        toggle_edit_button();
      }
      break;
    }
    case 1: { //modify temp settings
      if(clicked == 1){
        (s->getTemp() == 'F') ? s->setTemp('C') : s->setTemp('F');
        toggle_temp_settings();
      }else if(clicked == 2){
        state = 2;
        toggle_temp_black();
        toggle_dist_settings();
      }else{
        toggle_temp_settings();
      }
      break;
    }
    case 2: { //modify distance settings
      if(clicked == 0){
        state = 1;
        toggle_dist_black();
        toggle_temp_settings();
      }else if(clicked == 1){
        (s->getDist() == 'F') ? s->setDist('M') : s->setDist('F');
        toggle_dist_settings();
      }else if(clicked == 2){
        state = 3;
        toggle_dist_black();
        toggle_algo_settings();
      }else{
        toggle_dist_settings();
      }
      break;
    }
    case 3: { //modify algorithm settings
      if(clicked == 0){
        state = 2;
        toggle_algo_black();
        toggle_dist_settings();
      }else if(clicked == 1){
        (s->getAlgo() == 'C') ? s->setAlgo('L') : s->setAlgo('C');
        toggle_algo_settings();
      }else if(clicked == 2){
        state = 4;
        toggle_algo_black();
        toggle_done_button();
      }else{
        toggle_algo_settings();
      }
      break;
    }
    case 4: { //click done settings
      if(clicked == 0){
        state = 3;
        toggle_edit_done();
        toggle_algo_settings();
      }else if(clicked == 1){
        state = 0;
        s->write();
        toggle_edit_button();
      }else{
        toggle_done_button();
      }
      break;
    }
    case 5: { //view log book
      if(clicked == 0){
        state = 0;
        display_settings();
      }else if(clicked == 2){
        state = 6;
        display_plan_next_button();
      }
      break;
    }
    case 6: { //plan next dive button
      if(clicked == 0){
        state = 5;
        display_divelog(curr_dive_focus);
      }else if(clicked == 1){
        state = 7;
        display_plan_o2();
      }else if(clicked == 2){
        state = 10;
        display_descent_mode();
      }else{
        display_plan_next_button();
      }
      break;
    }
    case 7: { //set o2 level
      if(clicked == 0){
        if(nd->getO2() > 0){
          nd->setO2(nd->getO2()-1);
        }
        display_plan_o2();
      }else if(clicked == 1){
        state = 8;
        display_plan_depth();
      }else if(clicked == 2){
        if(nd->getO2() < 100){
          nd->setO2(nd->getO2()+1);
        }
        display_plan_o2();
      }
      break;
    }
    case 8: { //set max depth
      if(clicked == 0){
        if(nd->getDepth() > 0){
          nd->setDepth(nd->getDepth()-1);
        }
        display_plan_depth();
      }else if(clicked == 1){
        state = 9;
        display_plan_surftime();
      }else if(clicked == 2){
        if(nd->getDepth() < 250){
          nd->setDepth(nd->getDepth()+1);
        }
        display_plan_depth();
      }
      break;
    }
    case 9: { //set surf time
      if(clicked == 0){
        if(nd->getSurfTime() > 0){
          nd->setSurfTime(nd->getSurfTime()-1);
        }
        display_plan_surftime();
      }else if(clicked == 1){
        state = 6;
        nd->save();
        display_plan_next_button();
      }else if(clicked == 2){
        if(nd->getSurfTime() < 250){
          nd->setSurfTime(nd->getSurfTime()+1);
        }
        display_plan_surftime();
      }
      break;
    }
    case 10: { //display current dive descent mode
      if(clicked == 0){
        state = 6;
        display_plan_next_button();
      }else if(clicked == 1){
        state = 11;
        display_descent_description();
      }else if(clicked == 2){
        state = 12;
        display_ascent_mode();
      }else{
        display_descent_mode();
      }
      break;
    }
    case 11: { //display current dive descent mode description
      if(clicked == 0){
        state = 6;
        display_plan_next_button();
      }else if(clicked == 1){
        state = 10;
        display_descent_mode();
      }else if(clicked == 2){
        state = 12;
        display_ascent_mode();
      }
      break;
    }
    case 12: {  //display current dive ascent mode
      if(clicked == 0){
        state = 10;
        display_descent_mode();
      }else if(clicked == 1){
        state = 13;
        display_ascent_description();
      }else{
        display_ascent_mode();
      }
      break;
    }
    case 13: {  //display current dive ascent mode description
      if(clicked == 0){
        state = 10;
        display_descent_mode();
      }else if(clicked == 1){
        state = 12;
        display_ascent_mode();
      }
      break;
    }
  }
}

void handle_buttons(){
  int i;
  for(i=0; i < nbts; i++){
    if(!btgs[i]){
      if(digitalRead(bts[i])==LOW){
        btgs[i] = true;
      }
    }else{
      if(digitalRead(bts[i])==HIGH){
        btgs[i] = false;
        btnq->push(i);
//        btnq->toString(toPrint);
//        Serial.println(toPrint);
      }
    }
  }
  //delay(15);
}

/*
  SETTINGS UI FUNCTIONS
*/
void display_settings(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println("Settings");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(64, 10, 64, 30, WHITE);
  display.drawLine(0, 30, 128, 30, WHITE);
  display.drawLine(0, 47, 128, 47, WHITE);
  
  display.setTextSize(2);
  display.setCursor(25,15);
  display.println(s->getTemp());
  display.drawCircle(20, 17, 2, WHITE);
  
  display.setCursor(80,17);
  display.setTextSize(1);
  if(s->getDist() == 'F'){
    display.println(F("feet"));
  }else{
    display.println(F("meters"));
  }
  
  if(s->getAlgo() == 'C'){
    display.setCursor(30,35);
    display.println(F("conservative"));
  }else{
    display.setCursor(45,35);
    display.println(F("liberal"));
  }
  display.setCursor(51,54);
  display.fillRect(45, 52, 35, 10, WHITE);
  display.setTextColor(BLACK);
  display.println(F("EDIT"));
  
  
  display.display();
}

void toggle_edit_button(){
  if(millis()%1000 <= 500){
    display.setTextSize(1);
    display.setCursor(51,54);
    display.fillRect(45, 52, 35, 10, BLACK);
    display.setTextColor(WHITE);
    display.println("EDIT");
  }else{
    display.setTextSize(1);
    display.setCursor(51,54);
    display.fillRect(45, 52, 35, 10, WHITE);
    display.setTextColor(BLACK);
    display.println("EDIT");
  }
  display.display();
}
void toggle_edit_done(){
  display.setTextSize(1);
  display.setCursor(51,54);
  display.fillRect(45, 52, 35, 10, BLACK);
  display.setTextColor(WHITE);
  display.println("DONE");
  display.display();
}
void toggle_done_button(){
  if(millis()%1000 <= 500){
    display.setTextSize(1);
    display.setCursor(51,54);
    display.fillRect(45, 52, 35, 10, BLACK);
    display.setTextColor(WHITE);
    display.println("DONE");
  }else{
    display.setTextSize(1);
    display.setCursor(51,54);
    display.fillRect(45, 52, 35, 10, WHITE);
    display.setTextColor(BLACK);
    display.println("DONE");
  }
  display.display();
}

void toggle_temp_settings(){
  if(millis()%1000 <= 500){
    toggle_temp_black();
  }else{
    display.setTextSize(2);
    display.setCursor(25,15);
    display.fillRect(0, 11, 63, 18, WHITE);
    display.setTextColor(BLACK);
    display.println(s->getTemp());
    display.drawCircle(20, 17, 2, BLACK);
    display.display();
  }
}

void toggle_temp_black(){
  display.setTextSize(2);
  display.setCursor(25,15);
  display.fillRect(0, 11, 63, 18, BLACK);
  display.setTextColor(WHITE);
  display.println(s->getTemp());
  display.drawCircle(20, 17, 2, WHITE);
  display.display();
}
void toggle_dist_settings(){  
  if(millis()%1000 <= 500){
    toggle_dist_black();
  }else{
    display.setTextSize(1);
    display.setCursor(80,17);
    display.fillRect(65, 11, 60, 18, WHITE);
    display.setTextColor(BLACK);
    if(s->getDist() == 'F'){
      display.println("feet");
    }else{
      display.println("meters");
    }
    display.display();
  }
}
void toggle_dist_black(){
  display.setTextSize(1);
  display.setCursor(80,17);
  display.fillRect(65, 11, 60, 18, BLACK);
  display.setTextColor(WHITE);
  if(s->getDist() == 'F'){
    display.println("feet");
  }else{
    display.println("meters");
  }
  display.display();
}
void toggle_algo_settings(){
  if(millis()%1000 <= 500){
    toggle_algo_black();
  }else{
    display.setTextSize(1);
    display.fillRect(0, 31, 128, 16, WHITE);
    display.setTextColor(BLACK);
    if(s->getAlgo() == 'C'){
      display.setCursor(30,35);
      display.println("conservative");
    }else{
      display.setCursor(45,35);
      display.println("liberal");
    }
    display.display();
  }
}
void toggle_algo_black(){
  display.setTextSize(1);
  display.fillRect(0, 31, 128, 16, BLACK);
  display.setTextColor(WHITE);
  if(s->getAlgo() == 'C'){
    display.setCursor(30,35);
    display.println("conservative");
  }else{
    display.setCursor(45,35);
    display.println("liberal");
  }
  display.display();
}
/*
  LOGBOOK UI FUNCTIONS
*/
void display_divelog(int cdf){
  int count = d->getCount();
  char log_entry[24];
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(55,0);
  display.println("Log");
  display.drawLine(0, 10, 128, 10, WHITE);
  if(count == 0){
    display.setCursor(50,30);
    display.println("Empty");
  }else if(count == 1){
    display.setCursor(1,17);
    display.print("1:");
    d->getLog(0,log_entry);
    display.println(log_entry);
  }else if(count == 2){
    display.setCursor(1,17);
    display.print("1: ");
    d->getLog(0,log_entry);
    display.println(log_entry);
    display.drawLine(20, 30, 108, 30, WHITE);
    display.setCursor(1,37);
    display.print("2: ");
    d->getLog(1,log_entry);
    display.println(log_entry);
  }else{
    display.setCursor(1,17);
    display.print(curr_dive_focus-2);
    display.print(": ");
    d->getLog(curr_dive_focus-3,log_entry);
    display.println(log_entry);
    
    display.drawLine(20, 28, 108, 28, WHITE);
    display.setCursor(1,35);
    display.print(curr_dive_focus-1);
    display.print(": ");
    d->getLog(curr_dive_focus-2,log_entry);
    display.println(log_entry);
    
    display.drawLine(20, 46, 108, 46, WHITE);
    display.setCursor(1,53);
    display.print(curr_dive_focus);
    display.print(": ");
    d->getLog(curr_dive_focus-1,log_entry);
    display.println(log_entry);
  }
  display.display();
}

/*
 * PLAN NEXT DIVE UI
*/
void display_plan_next_button(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(38,0);
  display.setTextColor(WHITE);
  display.println(F("Dive Plan"));
  display.drawLine(0, 10, 128, 10, WHITE);
  display.setCursor(25,30);
  if(millis()%1000 <= 500){
    display.setTextColor(WHITE);
    if(nd->isPlanned()){
      display.println(F("Edit Next Dive"));
    }else{
      display.println(F("Plan Next Dive"));
    }
  }else{
    display.fillRect(20,25,93,17,WHITE);
    display.setTextColor(BLACK);
    if(nd->isPlanned()){
      display.println(F("Edit Next Dive"));
    }else{
      display.println(F("Plan Next Dive"));
    }
  }
  display.display();
}

void display_plan_o2(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println(F("O2 Level"));
  display.drawLine(0, 10, 128, 10, WHITE);
  display.setTextSize(2);
  display.setCursor(50,20);
  display.print((int)nd->getO2());
  display.print('%');
  display.setCursor(24,50);
  display.setTextSize(1);
  display.println(F("-     ok     +"));
  display.display();
}
void display_plan_depth(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.println(F("Max Depth"));
  display.drawLine(0, 10, 128, 10, WHITE);
  display.setTextSize(2);
  display.setCursor(42,20);
  display.print((int)nd->getDepth());
  if(s->getDist() == 'F'){
    display.print("ft");
  }else{
    display.print("m");
  }
  display.setCursor(24,50);
  display.setTextSize(1);
  display.println(F("-     ok     +"));
  display.display(); 
}
void display_plan_surftime(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(27,0);
  display.println(F("Surface Time"));
  display.drawLine(0, 10, 128, 10, WHITE);
  display.setTextSize(2);
  display.setCursor(30,20);
  display.print((int)nd->getSurfTime());
  display.print(" min");
  display.setCursor(18,50);
  display.setTextSize(1);
  display.println(F("-     save     +"));
  display.display(); 
}

/*
 * DESCENT MODE UI
*/
void display_descent_mode(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(26,0);
  display.println("Current Dive");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(0, 28, 128, 28, WHITE);
  display.drawLine(0, 46, 128, 46, WHITE);
  display.drawLine(60, 10, 60, 64, WHITE);
  //GTR
  display.setCursor(5,17);
  display.print(cd->getGTR());
  display.print(" min");

  //Air Pressure
  display.setCursor(70,17);
  display.print(cd->getPSI());
  display.print(" psi");

  //Depth
  display.setCursor(5,35);
  display.print(cd->getDepth());
  if(s->getDist() == 'F'){
    display.print(" ft");
  }else{
    display.print(" m");
  }

  //Max Depth
  display.setCursor(70,35);
  display.print(cd->getMaxDepth());
  if(s->getDist() == 'F'){
    display.print(" ft");
  }else{
    display.print(" m");
  }
  
  //Dive Time
  display.setCursor(5,53);
  display.print(cd->getDiveTime());
  display.print(" min");

  //Bottom Time
  display.setCursor(70,53);
  display.print(cd->getBottomTime());
  display.print(" min");
  
  display.display(); 
}

void display_descent_description(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(26,0);
  display.println("Descent Mode");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(0, 28, 128, 28, WHITE);
  display.drawLine(0, 46, 128, 46, WHITE);
  display.drawLine(60, 10, 60, 64, WHITE);
  //GTR
  display.setCursor(5,17);
  display.print("GTR");

  //Air Pressure
  display.setCursor(70,17);
  display.print("Air Press");

  //Depth
  display.setCursor(5,35);
  display.print("Depth");

  //Max Depth
  display.setCursor(70,35);
  display.print("Max Depth");
  
  //Dive Time
  display.setCursor(5,53);
  display.print("Dive Time");

  //Bottom Time
  display.setCursor(70,53);
  display.print("BTM Time");
  
  display.display(); 
}
/*
 * ASCENT MODE UI
*/
void display_ascent_mode(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(26,0);
  display.println("Current Dive");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(0, 28, 128, 28, WHITE);
  display.drawLine(0, 46, 128, 46, WHITE);
  display.drawLine(60, 10, 60, 46, WHITE);
  //GTR
  display.setCursor(5,17);
  display.print(cd->getGTR());
  display.print(" min");

  //Air Pressure
  display.setCursor(70,17);
  display.print(cd->getPSI());
  display.print(" psi");

  //Depth
  display.setCursor(5,35);
  display.print(cd->getDepth());
  if(s->getDist() == 'F'){
    display.print(" ft");
  }else{
    display.print(" m");
  }

  //ASCENT RATE
  display.setCursor(70,35);
  display.print(abs(cd->getAscentRate()));
  if(s->getDist() == 'F'){
    display.print(" ft/s");
  }else{
    display.print(" m/s");
  }
  if(cd->getAscentRate() > 0){
    display.drawLine(120, 40, 120, 33, WHITE);
    display.drawLine(117, 36, 120, 33, WHITE);
    display.drawLine(123, 36, 120, 33, WHITE);
  }else if (cd->getAscentRate() < 0){
    display.drawLine(120, 40, 120, 33, WHITE);
    display.drawLine(117, 37, 120, 40, WHITE);
    display.drawLine(123, 37, 120, 40, WHITE);
  }
  
  //Safety Stop
  display.setCursor(45,53);
  
  display.print(cd->getSafetyStop()/60);
  display.print(":");
  if(cd->getSafetyStop()%60 < 10){
    display.print("0");
  }
  display.print(cd->getSafetyStop()%60);
  
  display.display(); 
}

void display_ascent_description(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(26,0);
  display.println("Ascent Mode");
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(0, 28, 128, 28, WHITE);
  display.drawLine(0, 46, 128, 46, WHITE);
  display.drawLine(60, 10, 60, 46, WHITE);
  //GTR
  display.setCursor(5,17);
  display.print("GTR");

  //Air Pressure
  display.setCursor(70,17);
  display.print("Air Press");

  //Depth
  display.setCursor(5,35);
  display.print("Depth");

  //Max Depth
  display.setCursor(70,35);
  display.print("ASC Rate");
  
  //Safety Stop
  display.setCursor(30,53);
  display.print("Safety Stop");
  
  display.display(); 
}
