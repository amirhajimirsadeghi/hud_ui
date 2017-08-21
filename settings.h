#ifndef SETTINGS_H
#define SETTINGS_H

class Settings
{
     private :
          char temp;
          char dist;
          char algo;
     public :

          Settings();

          void setTemp(const char t);
          void setDist(const char d);
          void setAlgo(const char a);
          
          char getTemp() const;
          char getDist() const;
          char getAlgo() const;

          void write() const;
};
 
#endif