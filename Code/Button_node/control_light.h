#ifndef control_light_h_
#define control_light_h_

// #include <string.h>

class ControlLight
{
private:
  uint32_t light_id_ = 0;
  bool light_state_ = 0;

public:
  ControlLight(uint32 light_id);
  String setLightOn();
  String setLightOff();
  uint32_t getLightId();
};

#endif