#ifndef BTNQ_H
#define BTNQ_H

class BtnQ
{
     private :
          int queue[5] = {-1,-1,-1,-1,-1};
          int s_idx;
          int e_idx;
          bool empty = true;
     public :
          BtnQ();
          void push(const int x);
          int pop();
          void toString(char* tofill);
};
 
#endif