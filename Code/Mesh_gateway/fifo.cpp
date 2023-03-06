#include "Arduino.h"
#include "fifo.h"

using namespace std;

Fifo_string::Fifo_string()
{
  first_pos_ = 0;
  last_pos_ = 0;
  count_ = 0;
}
void Fifo_string::clear(char *s)
{
  while (*s)
  {
    *s = 0;
    s++;
  }
}

char *Fifo_string::get_element()
{
  if (isEmpty())
  {
    return 0;
  }

  char *result = _arr[first_pos_];
  first_pos_ = (first_pos_ + 1) % capacity_;
  count_--;

  return result;
}

void Fifo_string::push_element(String elem)
{
  if (isFull())
  {
    return;
  }

  char mess[1024];
  elem.toCharArray(mess, 1024);
  strcpy(_arr[last_pos_], mess);
  last_pos_ = (last_pos_ + 1) % capacity_;
  count_++;
}
void Fifo_string::push_element(char *elem)
{
  if (isFull())
  {
    return;
  }
  strcpy(_arr[last_pos_], elem);
  last_pos_ = (last_pos_ + 1) % capacity_;
  count_++;
}
void Fifo_string::push_element(uint8_t *elem)
{
  if (isFull())
  {
    return;
  }
  strcpy(_arr[last_pos_], (char *)elem);
  last_pos_ = (last_pos_ + 1) % capacity_;
  count_++;
}
bool Fifo_string::isEmpty()
{
  return (size() == 0);
}
bool Fifo_string::isFull()
{
  return (size() == capacity_);
}
int Fifo_string::size()
{
  return count_;
}
