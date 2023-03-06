
#include "Arduino.h"
#include "light.h"
#include "ArduinoJson.h"
#include "update_gen.h"
#include "painlessMesh.h"
painlessMesh mesh;

void Light::sendToHost(String mess)
{
	bool send = mesh.sendSingle(HOST_ID, mess);

	if (send)
	{
		Serial1.printf("Send %s to %u success\n", mess.c_str(), HOST_ID);
	}
	else
		Serial1.printf("Cannot send %s to %lu \n", mess.c_str(), HOST_ID);
}
Light::Light(uint8_t pin, int n_index, String name)
{
	this->index = n_index;
	this->light_pin_ = pin;
	this->light_name = name;
	pinMode(this->light_pin_, OUTPUT);
	turnOff();
}
void Light::turnOn()
{
	light_state_ = 1;
	digitalWrite(light_pin_, 1);
	String mess;
	mess = update_generate(light_name, "status", "ON", index);
	sendToHost(mess);
}

void Light::turnOff()
{
	light_state_ = 0;
	digitalWrite(light_pin_, 0);

	String mess;
	mess = update_generate(light_name, "status", "OFF", index);
	sendToHost(mess);
}

void Light::Excute(String cmd)
{
	if (cmd == "ON")
		turnOn();
	else if (cmd == "OFF")
		turnOff();
	else if (cmd == "TOGGLE")
		light_state_ ? turnOff() : turnOn();
}
bool Light::status()
{
	return light_state_;
}
String Light::get_name()
{
	return light_name;
}
