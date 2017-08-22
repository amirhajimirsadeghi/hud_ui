#include "next_dive.h"
#include <EEPROM.h>
using namespace std;
 
NextDive :: NextDive() {
	if(EEPROM.read(505) == 255){
		planned = false;
	}else{
		planned = true;
	}
	if(EEPROM.read(506) == 255){
		o2_level = 21;
		planned_depth = 60;
		surface_time = 60;
	}else{
		o2_level = EEPROM.read(506);
		planned_depth = EEPROM.read(507);
		surface_time = EEPROM.read(508);
	}
}
 
void NextDive :: setO2(const char t){o2_level = t;}
void NextDive :: setDepth(const char d){planned_depth = d;} 
void NextDive :: setSurfTime(const char a){surface_time = a;}

bool NextDive :: isPlanned() const { return planned;}
char NextDive :: getO2() const{return o2_level;}
char NextDive :: getDepth() const{return planned_depth;}
char NextDive :: getSurfTime() const{return surface_time;}

void NextDive :: save() {
	planned = true;
	EEPROM.write(505, 0);
	EEPROM.write(506, (unsigned char)o2_level );
	EEPROM.write(507, (unsigned char)planned_depth);
	EEPROM.write(508, (unsigned char)surface_time);
}
