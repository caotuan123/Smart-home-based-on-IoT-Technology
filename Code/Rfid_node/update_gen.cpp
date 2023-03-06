#include "update_gen.h"

String update_generate(String device, String info, String value, int index)
{
	DynamicJsonDocument doc(1024);
	String update_mess;

	doc["type"] = 4;
	doc["device"] = device;
	doc["value"] = value;
	doc["path"] = (String) "/list_device/[" + (String)index + "]/" + info;

	// serializeJson(doc, Serial);
	serializeJson(doc, update_mess);
	return update_mess;
}