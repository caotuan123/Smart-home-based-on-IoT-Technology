#ifndef MFRC522_CUSTOM_H
#define MFRC522_CUSTOM_H

#include "MFRC522.h"
#include "update_gen.h"
#include "painlessMesh.h"
#include "mesh.h"
#include "EEPROM.h"
#define HOST_ID 1305485767
#define GREEN_LED 0
#define RED_LED 15

class MFRC522_CUSTOM : public MFRC522
{

public:
  enum debugCode : uint8_t
  {
    OK,
    ERR_TIMEOUT,
    ERR_UID_IN_LIST,     // new uid already in list
    ERR_UID_NOT_IN_LIST, // this uid is not in list yet
    ERR_WRONG_CMD = 0xff
  };
  int index;
  uint8_t max_number_of_user = 10;
  uint64_t user_uid_list_[10] = {0};
  uint8_t user_uid_number_ = 0;
  bool Is_reading_new_tag_ = false;
  bool Is_deletting_tag_ = false;
  unsigned long TimeoutMillis_ = 0;
  MFRC522_CUSTOM(byte chipSelectPin, byte resetPowerDownPin, int n_index) : MFRC522(chipSelectPin, resetPowerDownPin)
  {
    this->index = n_index;
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    EEPROM.begin(8 * 10 + 1);
    first_readEEPROM();
  };

  void sendToHost(String mess);
  static const __FlashStringHelper *GetDebugCodeName(debugCode code);
  uint64_t transformUid(byte *buffer, byte bufferSize); // transform array Uid to 64byte uid
  void first_readEEPROM();
  bool update_EEPROM();
  debugCode readNewTag();
  debugCode deleteTag();
  void print_user_list();
  debugCode execute(String cmd);
  bool searchUser(uint64_t user_uid);
};

#endif