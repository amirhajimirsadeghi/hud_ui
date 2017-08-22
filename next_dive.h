#ifndef NEXTDIVE_H
#define NEXTDIVE_H

class NextDive
{
     private :
          bool planned;
          int o2_level;
          int planned_depth;
          int surface_time;
     public :

          NextDive();
          
          void setO2(const char t);
          void setDepth(const char d);
          void setSurfTime(const char a);
          
          bool isPlanned() const;
          char getO2() const;
          char getDepth() const;
          char getSurfTime() const;

          void save();
};
 
#endif