#include "control_light.h"

ControlLight::ControlLight(uint32_t light_id)
{
  this->light_id_ = light_id;
}

String ControlLight::setLightOn()
{
  this->light_state_ = 1;
  return "LIGHT_ON";
}

String ControlLight::setLightOff()
{
  this->light_state_ = 0;
  return "LIGHT_OFF";
}
uint32_t ControlLight::getLightId()
{
  return light_id_;
}