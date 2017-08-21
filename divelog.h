#ifndef DIVELOG_H
#define DIVELOG_H

class DiveLog
{
     private :
          int count;
          int toArray(int number ,char * l);
     public :

          DiveLog();

          int getCount();
          void getLog(int num, char* l);
          void addLog(int o2, int max_depth, int bottom_time, int day, int month, int year);
};
 
#endif