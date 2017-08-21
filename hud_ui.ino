#include <Adafruit_SSD1306.h>
#include "btnq.h"
#include "settings.h"
#include "divelog.h"

#define OLED_RESET 4

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
/*
 * EEPROM 1-3 : Settings
 * EEPROM 4 : LOGBOOK COUNT
 * EEPROM 5-9,10-14,...,500-504 -- up to 100 dives
*/
/* 
 * 0: settings(view), 1: settings(edit_temp), 2: settings(edit_dist), 3: settings(edit_algo), 4: settings(edit_done),
 * 5: 
*/
Adafruit_SSD1306 display(OLED_RESET);
int state = 0;

int nbts = 3;
int bts[3] = {2,4,7};
boolean btgs[3];
BtnQ* btnq;
char toPrint[6] = {'\0','\0','\0','\0','\0','\0'};  //debugging button queue

int curr_dive_focus;

Settings* s;
DiveLog* d;

void setup() {
  Serial.begin(9600);
  
  //data stuff
  s = new Settings();
  d = new DiveLog();
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
  int clicked = btnq->pop();
  //react based on button clicked
  switch(state){
    case 0: {
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
    case 1: {
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
    case 2: {
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
    case 3: {
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
    case 4: {
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
    case 5: {
      if(clicked == 0){
        state = 0;
        display_settings();
      }
      break;
    }
    case 6: {
      break;
    }
    case 7: {
      break;
    }
    case 8: {
      break;
    }
    case 9: {
      break;
    }
    case 10: {
      break;
    }
  }
}

void handle_buttons(){
  for(int i=0; i < nbts; i++){
    if(!btgs[i]){
      if(digitalRead(bts[i])==LOW){
        btgs[i] = true;
      }
    }else{
      if(digitalRead(bts[i])==HIGH){
        btgs[i] = false;
        btnq->push(i);
        btnq->toString(toPrint);
        Serial.println(toPrint);
      }
    }
  }
  delay(15);
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
    display.println("feet");
  }else{
    display.println("meters");
  }
  
  if(s->getAlgo() == 'C'){
    display.setCursor(30,35);
    display.println("conservative");
  }else{
    display.setCursor(45,35);
    display.println("liberal");
  }
  display.setCursor(51,54);
  display.fillRect(45, 52, 35, 10, WHITE);
  display.setTextColor(BLACK);
  display.println("EDIT");
  
  
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
/*
  
  d->getLog(0,log_entry);
  Serial.println(log_entry);
  Serial.println(log_entry);
*/
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

