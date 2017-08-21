#include "btnq.h"
using namespace std;
 
BtnQ :: BtnQ(){
	s_idx = 0;
	e_idx = 0;
}

void BtnQ :: push(const int x){
	if(empty){
		empty = false;
		queue[e_idx] = x;
		e_idx = (e_idx+1)%5;
	}else{
		if(e_idx == s_idx){
			//do nothing
		}else{
			queue[e_idx] = x;
			e_idx = (e_idx+1)%5;
	}
	}
		
}
int BtnQ :: pop(){
	if(s_idx == e_idx && empty){
		//do nothing
		return -1;
	}else{
		int to_ret = queue[s_idx];
		queue[s_idx] = -1;
		s_idx = (s_idx+1)%5;
		if(s_idx == e_idx){
			empty = true;
		}
		return to_ret;
	}
}
void BtnQ :: toString(char* tofill){
	for(int i; i<5; i++){
		if(queue[i] == -1){
			tofill[i] = '-';
		}else{
			char c = '0'+queue[i];
			tofill[i] = c;
		}
	}
}