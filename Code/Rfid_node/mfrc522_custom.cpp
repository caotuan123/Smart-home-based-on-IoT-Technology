#include "mfrc522_custom.h"

const __FlashStringHelper *MFRC522_CUSTOM::GetDebugCodeName(MFRC522_CUSTOM::debugCode code)
{
  switch (code)
  {
  case OK:
    return F("OKE");
  case ERR_TIMEOUT:
    return F("Time out error");
  case ERR_UID_IN_LIST:
    return F("This uid is already in list");
  case ERR_UID_NOT_IN_LIST:
    return F("this uid is not in list yet");
  default:
    return F("Unknown error");
  }
} // End GetStatusCodeName()
void MFRC522_CUSTOM ::sendToHost(String mess)
{
  bool send = mesh.sendSingle(HOST_ID, mess);

  if (send)
  {
    Serial.printf("Send %s to %u success\n", mess.c_str(), HOST_ID);
  }
  else
    Serial.printf("Cannot send %s to %lu \n", mess.c_str(), HOST_ID);
}

uint64_t MFRC522_CUSTOM::transformUid(byte *buffer, byte bufferSize)
{
  uint64_t medium_var = 0;
  uint64_t result_var = 0;
  for (int i = 0, j = bufferSize - 1; i < bufferSize; i++, j--)
  {
    medium_var = buffer[i];
    medium_var <<= j * 8;
    result_var |= medium_var;
  }
  return result_var;
}

MFRC522_CUSTOM::debugCode MFRC522_CUSTOM::readNewTag()
{

  Is_reading_new_tag_ = true;
  TimeoutMillis_ = millis();
  while ((Is_reading_new_tag_) && (TimeoutMillis_ + 60 * 1000 > millis()))
  {
    mesh.update();
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!PICC_IsNewCardPresent())
      continue;
    // Verify if the NUID has been readed
    if (!PICC_ReadCardSerial())
      continue;
    uint64_t temp_uid = transformUid(uid.uidByte, uid.size);
    for (int i = 0; i < user_uid_number_; i++)
    {
      if (user_uid_list_[i] == temp_uid)
      {

        Is_deletting_tag_ = false;
        return ERR_UID_IN_LIST;
      }
    }

    user_uid_list_[user_uid_number_] = temp_uid;
    user_uid_number_++;
    Is_reading_new_tag_ = false;
    update_EEPROM();
    return OK;
    // Halt PICC
    PICC_HaltA();
    // Stop encryption on PCD
    PCD_StopCrypto1();
  }
  Is_reading_new_tag_ = false;
  return ERR_TIMEOUT;
}

MFRC522_CUSTOM::debugCode MFRC522_CUSTOM::deleteTag()
{
  Is_deletting_tag_ = true;
  TimeoutMillis_ = millis();
  while ((Is_deletting_tag_) && (TimeoutMillis_ + 60 * 1000 > millis()))
  {
    mesh.update();
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if (!PICC_IsNewCardPresent())
      continue;
    // Verify if the NUID has been readed
    if (!PICC_ReadCardSerial())
      continue;
    uint64_t temp_uid = transformUid(uid.uidByte, uid.size);
    for (int i = 0; i < user_uid_number_; i++)
    {
      if (user_uid_list_[i] == temp_uid)
      {
        for (int j = i; j < user_uid_number_; j++)
        {
          user_uid_list_[j] = user_uid_list_[j + 1];
        }
        Is_deletting_tag_ = false;
        user_uid_number_--;
        update_EEPROM();
        return OK;
      }
    }
    // Halt PICC
    PICC_HaltA();
    // Stop encryption on PCD
    PCD_StopCrypto1();

    Is_deletting_tag_ = false;
    return ERR_UID_NOT_IN_LIST;
  }
  Is_deletting_tag_ = false;
  return ERR_TIMEOUT;
}
void MFRC522_CUSTOM::print_user_list()
{
  for (int i = 0; i < user_uid_number_; i++)
  {
    Serial.println(user_uid_list_[i], HEX);
  }
}
MFRC522_CUSTOM::debugCode MFRC522_CUSTOM::execute(String cmd)
{
  debugCode tmp;
  if (cmd == "NEW")
  {
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("Start creat new user");
    sendToHost(update_generate("rfid", "is_crt", "1", index));
    Serial.println("Update is creating new user");
    tmp = readNewTag();
    // print_user_list();
    switch (tmp)
    {
    case OK:
      sendToHost(update_generate("rfid", "crt_noti", "SUCCESS", index));
      break;
    case ERR_TIMEOUT:
      sendToHost(update_generate("rfid", "crt_noti", "TIME_OUT", index));
      break;
    case ERR_UID_IN_LIST:
      sendToHost(update_generate("rfid", "crt_noti", "FAILED", index));
      break;
    default:
      break;
    }
    sendToHost(update_generate("rfid", "is_crt", "0", index));
    digitalWrite(GREEN_LED, LOW);

    return tmp;
  }
  else if (cmd == "DELETE")
  {
    digitalWrite(RED_LED, HIGH);
    sendToHost(update_generate("rfid", "is_del", "1", index));
    tmp = deleteTag();
    print_user_list();
    switch (tmp)
    {
    case OK:
      sendToHost(update_generate("rfid", "del_noti", "SUCCESS", index));
      break;
    case ERR_TIMEOUT:
      sendToHost(update_generate("rfid", "del_noti", "TIME_OUT", index));
      break;
    case ERR_UID_NOT_IN_LIST:
      sendToHost(update_generate("rfid", "del_noti", "FAILED", index));
      break;
    default:
      break;
    }
    digitalWrite(RED_LED, LOW);
    sendToHost(update_generate("rfid", "is_del", "0", index));
    return tmp;
  }
  else
    return ERR_WRONG_CMD;
}
bool MFRC522_CUSTOM::searchUser(uint64_t user_uid)
{
  for (int i = 0; i < user_uid_number_; i++)
  {
    if (user_uid == user_uid_list_[i])
      return true;
  }
  return false;
}
void MFRC522_CUSTOM::first_readEEPROM()
{
  uint8_t size;
  uint64_t data = 0;
  EEPROM.get(0, size);
  Serial.printf("Size of first list: %d:\n ", size);
  if (size == 0 || size > max_number_of_user)
    return;
  else
  {
    for (uint8_t i = 0; i < size; i++)
    {
      EEPROM.get(1 + i * 8, data);
      user_uid_list_[i] = data;
    }
    user_uid_number_ = size;
  }
}
bool MFRC522_CUSTOM::update_EEPROM()
{
  Serial.printf("User number push to ROM: %d:\n ", user_uid_number_);
  EEPROM.put(0, user_uid_number_);
  for (uint8_t i = 0; i < user_uid_number_; i++)
  {
    EEPROM.put(1 + i * 8, user_uid_list_[i]);
    if (!EEPROM.commit())
      return false;
  }
  return true;
}