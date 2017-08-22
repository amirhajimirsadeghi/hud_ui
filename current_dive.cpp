#include "current_dive.h"
using namespace std;

CurrentDive :: CurrentDive() {}

int CurrentDive :: getPSI() const {return 2500;}
int CurrentDive :: getGTR() const {return 25;}
int CurrentDive :: getDepth() const {return 30;}
int CurrentDive :: getMaxDepth() const {return 60;}
int CurrentDive :: getDiveTime() const {return 35;}
int CurrentDive :: getBottomTime() const {return 15;}
int CurrentDive :: getAscentRate() const {return 15;}
int CurrentDive :: getSafetyStop() const {return 900;}