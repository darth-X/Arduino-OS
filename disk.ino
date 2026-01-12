/*
  Disk driver

  -> 2047 max address
*/

//writes to disk
void writeDisk(unsigned short lba, byte data) {
  byte block = 0b000;
  const unsigned short lba_init = lba;
  
  while(true){
    if(lba <= 255){
      break;
    }

    lba -= 256;
    block += 0b001;
  }

  if(readDisk(lba_init) != data){
    Wire.beginTransmission((0b1010 << 3) + block);
    Wire.write(lba);
    Wire.write(data);
    Wire.endTransmission();
    delay(5);
  }

  return;
}

//reads from disk
byte readDisk(unsigned int lba) {
  byte data;
  byte block = 0b000;

  while(true){
    if(lba <= 255){
      break;
    }

    lba -= 256;
    block += 0b001;
  }
  
  Wire.beginTransmission((0b1010 << 3) + block);
  Wire.write(lba);
  Wire.endTransmission();
  Wire.requestFrom((0b1010 << 3) + block, 1);
  delay(1);
  if (Wire.available()) {
    data = Wire.read();
  }
  return data;
}

unsigned short fileCheck(void) {
  unsigned short addr = 0;
  byte data;

  while (true) {
    if (addr >= lbaMAX) {
      return 0;
    }

    data = readDisk(addr);

    if (data == endBit && addr != 0) {
      return addr;
    }

    addr++;
  }
}

bool diskPresent(){
  Wire.beginTransmission(0b1010000);
  
  if(Wire.endTransmission() == 0){
    return true;
  }

  return false;
}

void programEEPROM(void) {
  lcd.clear();
  lcd.home();

  if(!diskPresent()){
    lcd.print(F("Disk is not"));
    lcd.setCursor(0, 1);
    lcd.print(F("present!"));
    delay(1000);
    updatePAGE();
    return;
  }

  delay(500);

  int overwrite = 0;
  const char* const YN_str[] = {"NO", "YES"};
  int section_number = 1;

  lcd.print(F("SELECT SECTOR:"));

  // getting "which sector"
  lastStateCLK = digitalRead(CLK);
  while (true) {
    input_counter(&section_number, 1, 4);
    lcd.setCursor(0, 1);
    lcd.print(section_number);

    if (!digitalRead(select)) break;
  }

  lcd.clear();
  delay(250);
  lcd.home();
  lcd.print(F("OVERWRITE?:"));

  // getting "overwrite?"
  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&overwrite, 0, 1);
    lcd.setCursor(0, 1);
    lcd.print(YN_str[overwrite]);

    if(!digitalRead(select)) break;
  }

  
  if (overwrite == 1) {
    prog(0);
    lcd.clear();
    lcd.home();
    delay(500);
    updatePAGE();
    return;
  } else {
    int head = fileCheck();
    if(head > 0){
      prog(head);
      updatePAGE();
      return;
    } else {
      lcd.clear();
      lcd.home();
      lcd.print(F("Bad file format!"));
      delay(1000);
      updatePAGE();
      return;
    }
  }
}

void readEEPROM(void) {
  lcd.home();
  lcd.clear();

  if(!diskPresent()){
    lcd.print(F("Disk is not"));
    lcd.setCursor(0, 1);
    lcd.print(F("present!"));
    delay(1000);
    updatePAGE();
    return;
  }

  if(devmode){
    unsigned short i = 0;
    while(i <= lbaMAX){
      Serial.print(i);
      Serial.print(F(" : "));
      Serial.println(readDisk(i));
      i++;
    }
  }
  byte data = readDisk(getLBA());
  lcd.print(F("DATA: "));
  lcd.setCursor(0, 1);
  lcd.print(data);
  delay(1000);
  updatePAGE();
}

void writeRAW_EEPROM(void){
  lcd.clear();
  lcd.home();

  if(!diskPresent()){
    lcd.print(F("Disk is not"));
    lcd.setCursor(0, 1);
    lcd.print(F("present!"));
    delay(1000);
    updatePAGE();
    return;
  }
  
  writeDisk(getLBA(), getRAW());
  lcd.clear();
  lcd.home();
  lcd.print(F("Writen!"));
  delay(500);
  updatePAGE();
}
