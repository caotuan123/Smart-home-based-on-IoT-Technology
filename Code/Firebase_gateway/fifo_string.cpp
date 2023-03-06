#include "Arduino.h"
#include "fifo_string.h"

using namespace std;


Fifo_string::Fifo_string()
{
  //char _arr[size][1024];
  //capacity_=10;
  first_pos_=0;
  last_pos_=0;
  count_=0;
}
void Fifo_string::clear(char *s){
  while(*s)
  {
    *s=0;
    s++;
  }
}
String Fifo_string::get_element(){
  if(isEmpty())
  {
    return "0";
  }
  String result=_arr[first_pos_];
  clear(_arr[first_pos_]);
  //Serial.printf("removing : %s\n",result);
  //Serial.printf("Item number left in FIFO is %d\n",this->count_);
  first_pos_=(first_pos_+1) %capacity_;
  count_--;
  //Serial.printf("Remove %s succes\n",result);
  return result;
}

void Fifo_string::push_element(String elem){
  if (isFull())
  {
    return;
  }
    
  //Serial.printf("inserting item : %s\n",elem.c_str());
  char mess[1024];
  elem.toCharArray(mess,1024);
  strcpy(_arr[last_pos_],mess);
  last_pos_=(last_pos_+1)%capacity_;
  count_++;
} 
bool Fifo_string::isEmpty(){
  return(size()==0);
}
bool Fifo_string::isFull(){
  return(size()==capacity_);
}
int Fifo_string::size(){
  return count_;
  
}

