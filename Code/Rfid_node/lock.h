#ifndef lock_h_
#define lock_h_

#include "update_gen.h"
#include "ArduinoJson.h"
#include "painlessMesh.h"
#include "mesh.h"
#define HOST_ID 1305485767

class Lock
{
private:
  int index;
  uint8_t lock_pin_;
  uint8_t lock_state_ = 0;          // 1=open, 0=close
  unsigned long TimeoutMillis_ = 0; // in milli
  int SetTimeOut_ = 5000;

public:
  Lock(uint8 lock_pin, int index);
  void sendToHost(String mess);
  int IsLockOpen();
  void LockClose();
  void LockOpen();
  unsigned long getTimeOut();
  void excute(String);
};

#endif