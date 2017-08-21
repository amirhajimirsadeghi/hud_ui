#include "settings.h"
#include <EEPROM.h>
using namespace std;
 
Settings :: Settings() {
	if(EEPROM.read(1) == 255){
		temp = 'F';
		EEPROM.write(1, 'F');
		dist = 'F';
		EEPROM.write(2, 'F');
		algo = 'C';
		EEPROM.write(3, 'C');
	}else{
		temp = EEPROM.read(1);
		dist = EEPROM.read(2);
		algo = EEPROM.read(3);
	}
}
 
void Settings :: setTemp(const char t){temp = t;}
void Settings :: setDist(const char d){dist = d;} 
void Settings :: setAlgo(const char a){algo = a;}

char Settings :: getTemp() const{
     return temp;
}
char Settings :: getDist() const{
     return dist;
     
}
char Settings :: getAlgo() const{
     return algo;
}

void Settings :: write() const {
	EEPROM.write(1, getTemp());
	EEPROM.write(2, getDist());
	EEPROM.write(3, getAlgo());
}
