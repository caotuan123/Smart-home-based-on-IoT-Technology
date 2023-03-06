#ifndef _fifo_string_h_
#define _fifo_string_h_

#include <Arduino.h>
#define FIFO_LENGTH 10
class Fifo_string
{
private:
    char _arr[FIFO_LENGTH][1024];
    int capacity_=FIFO_LENGTH;
    int first_pos_;
    int last_pos_;
    int count_;

public:
    Fifo_string();
    void clear(char*s);
    String get_element();
    void push_element(String elem);
    bool isEmpty();
    bool isFull();
    int size();

};

#endif 