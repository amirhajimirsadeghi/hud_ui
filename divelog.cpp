#include "divelog.h"
#include <EEPROM.h>
#include <math.h>
using namespace std;

DiveLog :: DiveLog() {
	if(EEPROM.read(4) == 255){
		count = 0;
		EEPROM.write(4, 0);
	}else{
		count = EEPROM.read(4);
	}
}

int DiveLog :: getCount(){
	return count;
}

void DiveLog :: getLog(int num, char* l){
	int next_index = 0;
	if(num < count){
		next_index += toArray((int)EEPROM.read(0 + ((num%100)+1)*5), l);
		l[next_index] = '%';
		l[next_index+1] = ' ';
		l[next_index+2] = ' ';
		next_index += 3;
		next_index += toArray((int)EEPROM.read(1 + ((num%100)+1)*5), &l[next_index]);
		l[next_index] = ' ';
		l[next_index+1] = ' ';
		next_index += 2;
		next_index += toArray((int)EEPROM.read(2 + ((num%100)+1)*5), &l[next_index]);
		l[next_index] = ' ';
		l[next_index+1] = ' ';
		next_index += 2;

		unsigned char day_month = EEPROM.read(3 + ((num%100)+1)*5);
		unsigned char month_year = EEPROM.read(4 + ((num%100)+1)*5);
		next_index += toArray((int)(((day_month &0x7)<<1)|((month_year&0x80)>>7)) , &l[next_index]);
		l[next_index] = '/';
		next_index += 1;
		next_index += toArray((int)((day_month & 0xF8)>>3) , &l[next_index]);
		//l[next_index] = '/';
		//next_index += 1;
		// next_index += toArray((int)(month_year & 0x7F) , &l[next_index]);
		l[next_index] = '\0';
	}
}

int DiveLog :: toArray(int number ,char * l){
    int n = log10(number) + 1;
    for (int i = n-1; i >= 0; --i, number /= 10 ){
        l[i] = (number % 10) + '0';
    }
    return n;
}

void DiveLog :: addLog(int o2, int max_depth, int bottom_time, int day, int month, int year){
	//[][][][][][][][] [][][][][][][][] [][][][][][][][] [][][][][][][][] [][][][][][][][]
	//     o2              max_depth        bottom_time  d d d d d m m m  m y y y y y y y
	
	unsigned char day_month = (day &  0x1F) << 3 ;	
	day_month = day_month | (month & 0xF) >> 1; 
	unsigned char month_year =  (month & 0x1) <<  7; 
	month_year = month_year | (year & 0x7F);
	if(count >= 100){
		EEPROM.write((0 + ((count%100)+1)*5), (char)o2);
		EEPROM.write((1 + ((count%100)+1)*5), (char)max_depth);
		EEPROM.write((2 + ((count%100)+1)*5), (char)bottom_time);
		EEPROM.write((3 + ((count%100)+1)*5), day_month);
		EEPROM.write((4 + ((count%100)+1)*5), month_year);
	}else{

		EEPROM.write((0 + (count+1)*5), (char)o2);
		EEPROM.write((1 + (count+1)*5), (char)max_depth);
		EEPROM.write((2 + (count+1)*5), (char)bottom_time);
		EEPROM.write((3 + (count+1)*5), day_month);
		EEPROM.write((4 + (count+1)*5), month_year);
	}
	count++;
	EEPROM.write(4, count);
}