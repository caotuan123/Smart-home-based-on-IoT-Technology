#include "Arduino.h"
#include "fifo_string.h"

using namespace std;
FifoString::FifoString(int size)
{
  arr_ = new String[size];
  capacity_ = size;
  first_pos_ = 0;
  last_pos_ = 0;
  count_ = 0;
}

String FifoString::get_element()
{
  if (isEmpty())
  {
    return "0";
  }
  String result = arr_[first_pos_];
  Serial.printf("removing : %s\n", result.c_str());
  Serial.printf("Item number left in FIFO is %d\n", this->count_);
  first_pos_ = (first_pos_ + 1) % capacity_;
  count_--;
  return result;
}

void FifoString::push_element(String elem)
{
  if (isFull())
  {
    return;
  }
  Serial.printf("inserting item : %s\n", elem.c_str());
  arr_[last_pos_] = elem;
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
