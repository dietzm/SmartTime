//Define the pin connection
int CLK = 2;//CLK->D2
int DT = 4;//DT->D3
int SW = 3;//SW->D4
const int interrupt0 = 0;// Interrupt 0 在 pin 2 上
int count = 0;//Define the count
int lastCLK = 0;//CLK initial value
boolean pressBTN=false;

void initEncoder(){
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  int intr = digitalPinToInterrupt(CLK);
  int intr2 = digitalPinToInterrupt(SW);
  Serial.print("Int:");
  Serial.println(intr2);
  attachInterrupt(intr, ClockChanged, CHANGE);//Set the interrupt 0 handler, trigger level change
  attachInterrupt(intr2, pressed, CHANGE);//Set the interrupt 1 handler, trigger level change
}

boolean ispressed(){
  if(pressBTN){
    pressBTN = false;
    return true;
  }
   return false;
}


volatile unsigned long  lastPress = 0;
void pressed(){
    unsigned long currMillis = millis();
  if (currMillis - lastPress < 500) {
      return;
  }
  pressBTN = true;
  lastPress=currMillis;
  if(pressBTN){
    Serial.println("pressed TRUE");
  }else{
    Serial.println("pressed FALSE");
  }
}

int getvalue(){
  return abs(count);
}

int setvalue(int val){
  count=val;
}

volatile unsigned long lastEncoding = 0;
//The interrupt handlers
void ClockChanged()
{
cli(); //stop interrupts happening before we read pin values
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  unsigned long currMillis = millis();


  if (lastCLK != clkValue)
  {
    lastCLK = clkValue;

    if(currMillis - lastEncoding > 5){
      // Serial.print(clkValue);
      // Serial.print(':');
      // Serial.println(dtValue);
      // Serial.println(currMillis - lastEncoding);
    count += (clkValue != dtValue ? 1 : -1);//CLK and inconsistent DT + 1, otherwise - 1

    Serial.print("count:");
    Serial.println(abs(count));
    lastEncoding = currMillis;
  }else{
    // Serial.print('X');
    // Serial.print(clkValue);
    // Serial.print(':');
    // Serial.println(dtValue);
    // Serial.println(currMillis - lastEncoding);
  }
  }
   sei(); //restart interrupts
}
