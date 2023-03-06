#ifndef _fifo_string_h_
#define _fifo_string_h_

#include <Arduino.h>
#define FIFO_LENGTH 5
// The FifoString class creates a FIFO to hold messages that have
// not yet been processed by the node.
class FifoString
{
private:
    char arr_[10][1024];
    int capacity_ = 10;
    int first_pos_;
    int last_pos_;
    int count_;

public:
    FifoString();
    void clear(char *s);
    String get_element();
    void push_element(String elem);
    bool isEmpty();
    bool isFull();
    int size();
};

#endif