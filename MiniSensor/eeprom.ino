/*****************************************************************************/
/*****************************  write to EEPROM  *****************************/
void writeString(int address, String data)
{
  int dataSize = data.length();
  int i;
  initialCheck.detach();
  //buttonCheck.detach();
  //sendDataTicker.detach();
  for (i = 0; i < dataSize; i++)
  {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + dataSize, '\0'); //Add termination null character for String Data
  EEPROM.commit();

  initialCheck.attach(240, changeinitialCheck);
  //buttonCheck.attach(0.2, FindbuttonCheck);
  //sendDataTicker.attach(5, sendDataVoid);
}
/*****************************  write to EEPROM  *****************************/
/*****************************************************************************/


/*****************************************************************************/
/*****************************  read from EEPROM  ****************************/
String read_String(int address)
{
  char data[100]; //Max 100 Bytes
  char k = ' ';
  for (int len = 0; k != '\0' && len < 100 && k != 0xFF; len++) //Read until null character
  {
    k = EEPROM.read(address + len);
    data[len] = k;
  }
  return String(data);
}
/*****************************  read from EEPROM  ****************************/
/*****************************************************************************/
