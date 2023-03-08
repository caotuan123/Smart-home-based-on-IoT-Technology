#ifndef _fifo_string_hpp_
#define _fifo_string_hpp_

#include <Arduino.h>
// The FifoString class creates a FIFO to hold messages that have
// not yet been processed by the node.
class FifoString
{
private:
    String *arr_;
    int capacity_;
    int first_pos_;
    int last_pos_;
    int count_;

public:
    FifoString(int size);
    String get_element();
    void push_element(String elem);
    bool isEmpty();
    bool isFull();
    int size();
};

#endif