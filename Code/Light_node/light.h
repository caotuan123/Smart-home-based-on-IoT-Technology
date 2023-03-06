#ifndef light_hpp_
#define light_hpp_

#define HOST_ID 1305485767

class Light
{
private:
  bool light_state_ = 0;
  uint8_t light_pin_;
  int index;
  String light_name;

public:
  Light(uint8_t pin, int index, String name);
  void sendToHost(String mess);
  void turnOn();
  void turnOff();
  bool status();
  void Excute(String);
  String get_name();
};

#endif
