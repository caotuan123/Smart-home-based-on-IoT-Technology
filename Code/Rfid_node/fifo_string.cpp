#include "Arduino.h"
#include "fifo_string.h"

using namespace std;
// FifoString class create an FIFO to hold message that not yet process by the node
FifoString::FifoString()
{
  first_pos_ = 0;
  last_pos_ = 0;
  count_ = 0;
}

void FifoString::clear(char *s)
{
  while (*s)
  {
    *s = 0;
    s++;
  }
}

String FifoString::get_element()
{
  if (isEmpty())
  {
    return "0";
  }
  String result = arr_[first_pos_];
  clear(arr_[first_pos_]);
  // Serial.printf("removing : %s\n",result);
  // Serial.printf("Item number left in FIFO is %d\n",this->count_);
  first_pos_ = (first_pos_ + 1) % capacity_;
  count_--;
  // Serial1.printf("Remove %s succes\n",result);
  return result;
}

void FifoString::push_element(String elem)
{
  if (isFull())
  {
    return;
  }

  // Serial1.printf("inserting item : %s\n",elem);
  char mess[1024];
  elem.toCharArray(mess, 1024);
  strcpy(arr_[last_pos_], mess);
  last_pos_ = (last_pos_ + 1) % capacity_;
  count_++;
}
bool FifoString::isEmpty()
{
  return (size() == 0);
}
bool FifoString::isFull()
{
  return (size() == capacity_);
}
int FifoString::size()
{
  return count_;
}
