#ifndef CURRENTDIVE_H
#define CURRENTDIVE_H

class CurrentDive
{
     private :
     public :
          CurrentDive();
          
          int getPSI() const;
          int getGTR() const;
          int getDepth() const;
          int getMaxDepth() const;
          int getDiveTime() const;
          int getBottomTime() const;
          int getAscentRate() const;
          int getSafetyStop() const;
};
 
#endif