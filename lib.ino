/*
  Support functions
*/

#define debounce 250

/*===== User input stuff =====*/
byte getDat(void) {
  delay(debounce);
  lcd.clear();
  lcd.home();
  lcd.print(F("DATA:"));
  lcd.setCursor(0, 1);
  lcd.print(F("a"));
  int count = 33;

  lastStateCLK = digitalRead(CLK);
  while (true) {
    input_counter(&count, 33, 125);
    lcd.setCursor(0, 1);

    if(count == 92){
      lcd.write(byte(0));
    } else {
      lcd.print((char)count);
    }

    lcd.print(F(" "));

    if (digitalRead(select) == 0) {
      return count;
    }
  }
}

byte getNum(byte high, byte low){
  int count = 0;
  delay(debounce);
  lcd.clear();
  lcd.home();
  lcd.print(F("NUM:"));
  lcd.setCursor(0, 1);
  lcd.print(low);
  
  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, low, high);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("   "));
    
    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getcmd(void){
  delay(debounce);
  lcd.clear();
  lcd.home();
  lcd.print(F("CMD:"));
  lcd.setCursor(0, 1);
  lcd.print(commands_inter[0]);

  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, commandMAX);
    lcd.setCursor(0, 1);
    lcd.print(commands_inter[count]);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

byte getRAW(void){
  delay(debounce);
  lcd.clear();
  lcd.home();
  lcd.print(F("RAW:"));
  lcd.setCursor(0, 1);
  lcd.print(F("0"));

  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, 255);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

unsigned short getLBA(void){
  delay(debounce);
  lcd.clear();
  lcd.home();
  lcd.print(F("LBA: "));
  lcd.setCursor(0, 1);
  lcd.print(F("0"));
  
  int count = 0;

  lastStateCLK = digitalRead(CLK);
  while(true){
    input_counter(&count, 0, lbaMAX);
    lcd.setCursor(0, 1);
    lcd.print(count);
    lcd.print(F("    "));

    if(digitalRead(select) == 0){
      return count;
    }
  }
}

/*===== Other helpers =====*/
void input_counter(int* previous_count, int min, int max){
  currentStateCLK = digitalRead(CLK);

  if(currentStateCLK != lastStateCLK){
    if(digitalRead(DT) != currentStateCLK){
      if(*previous_count < max) (*previous_count)++;
      else *previous_count = min;
    } else if(digitalRead(DT) == currentStateCLK){
      if(*previous_count > min) (*previous_count)--;
      else *previous_count = max;
    }

    lastStateCLK = currentStateCLK;
  }
}

/*===== Page updater =====*/
int last_cursor_pos = 0;

void updatePAGE(){
  unsigned int groupNo = 1;

  // find grp
  while((MAX_Y * groupNo) < opt) groupNo++;

  // print accordingly
  for(byte i = 0; i <= (MAX_Y - 1); i++){
    lcd.setCursor(0, i);
    lcd.print(page[MAX_Y * (groupNo - 1) + i + 1]);
    for(byte ii = 1; ii < (MAX_X - sizeof(page[MAX_Y * (groupNo - 1) + i])); ii++) lcd.print(F(" "));
  }

  // delete last cursor postition
  lcd.setCursor((MAX_X - 1), last_cursor_pos);
  lcd.print(F(" "));
  last_cursor_pos = (MAX_Y - 1) - ((groupNo * MAX_Y) - opt);
  
  // update cursor location
  lcd.setCursor((MAX_X - 1), (MAX_Y - 1) - ((groupNo * MAX_Y) - opt));
  lcd.print(F("<"));

}