#include "lock.h"

void Lock::sendToHost(String mess)
{
  bool send = mesh.sendSingle(HOST_ID, mess);
  if (send)
  {
    Serial.printf("Send %s to %u success\n", mess.c_str(), HOST_ID);
  }
  else
    Serial.printf("Cannot send %s to %lu \n", mess.c_str(), HOST_ID);
}
Lock::Lock(uint8_t lock_pin, int index)
{
  this->index = index;
  this->lock_pin_ = lock_pin;
  pinMode(lock_pin_, OUTPUT);
  digitalWrite(lock_pin_, HIGH);
}

void Lock::LockClose()
{
  lock_state_ = 0;
  digitalWrite(lock_pin_, HIGH);
  TimeoutMillis_ = 0;
  sendToHost(update_generate("lock", "status", "CLOSE", index));
}
void Lock::LockOpen()
{
  lock_state_ = 1;
  digitalWrite(lock_pin_, LOW);
  TimeoutMillis_ = millis() + SetTimeOut_;
  sendToHost(update_generate("lock", "status", "OPEN", index));
}
int Lock::IsLockOpen()
{
  return lock_state_;
}
unsigned long Lock::getTimeOut()
{
  return TimeoutMillis_;
}
void Lock::excute(String cmd)
{
  if (cmd == (String) "OPEN")
  {
    LockOpen();
  }
  else if (cmd == (String) "CLOSE")
  {
    LockClose();
  }
  else
    Serial.println("Wrong command!");
}