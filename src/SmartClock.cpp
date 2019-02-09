#include <Arduino.h>
#include "MyDisplay.h"
#include "rotaryencoder.h"
#include <DS3231.h>
#include "SmartClock.h"

int mode=0;
DS3231 clock;
RTCDateTime dtime;
boolean bar=true;
int intensity;
int alarmhour,alarmmin;
boolean alarm = false;
int cnt=0;
String message="Marie";

extern String btreceive();
extern void btsend(String msg);

void setupClock(){
  clock.begin();
}

int readNumericValue(){
  String recv = btreceive();
  if(recv.equals("1")){
        setvalue(2);
  }
  if(recv.equals("2")){
        setvalue(4);
  }
  if(recv.equals("3")){
        setvalue(6);
  }
  if(recv.equals("4")){
        setvalue(8);
  }
  if(recv.equals("5")){
        setvalue(10);
  }
  if(recv.equals("6")){
        setvalue(12);
  }
  if(recv.equals("7")){
        setvalue(14);
  }
  if(recv.equals("8")){
        setvalue(16);
  }
  if(recv.equals("9")){
        setvalue(18);
  }
  if(recv.equals("0")){
        setvalue(0);
  }
  if(recv.equals('!')){
    pressed();
  }
  return getvalue();
}

boolean menuSelect(){
  //buzzer();
  //btreceive();
  int val = readNumericValue()/2%7;
  if(val != mode || mode < 1){
    //Show Menu
    mode=0;
    switch(val){
     case 0:
          paintText(1,"Menu");
          break;
     case 1:
          paintText(1,"Uhr");
          break;
     case 2:
          paintText(1,"Timer");
          break;
     case 3:
          paintText(1,"Stoppuhr",true);
          break;
     case 4:
          paintText(1,"Zocken");
          break;
     case 5:
          paintText(1,"Text");
          break;
     case 6:
           paintText(1,"Optionen",true);
           break;
    }
    if(ispressed()){
      mode=val;
      cnt=0;
      clearDisplay();
    }else{
      if(cnt++>100){ //menu timeout
          cnt=0;
          clearDisplay();
          setvalue(2);//Uhr
          mode=1;
      }
      return false;
    }
  }
  return true;
}

void menuOptions(){
  ispressed(); // eat double presses
  int pos=5;
  while(true){
    if(pos<-12){
      pos=5;
    }
    int val = getvalue()/2%4;
    //Show Menu
    switch(val){
      case 0:
      if(bar){
        paintText(1,"Leiste {",true);
      }else{
        paintText(1,"Leiste x",true);
      }
      if(ispressed()){
        bar = !bar;
        continue;
      }
      break;
      case 1:
      paintText(pos--,"Uhr stellen",true);

      if(ispressed()){
        clearDisplay();
        int nr[4];
        eingabe(nr,4);
        dtime = clock.getDateTime();
        int hour=nr[0]*10+nr[1];
        int mini=nr[2]*10+nr[3];
        clock.setDateTime(dtime.year,dtime.month,dtime.day,hour,mini,0);
        mode=0;
      }

      break;
      case 2:
      paintText(pos--,"Helligkeit",true);
      if(ispressed()){
        intensity++;
        if(intensity>=9){
          intensity=1;
        }
        setIntensity(intensity);
      }
      break;
      case 5:
      paintText(1,"Opt");
      break;
      case 4:
      paintText(1,"Opt4");
      break;
      case 3:
      paintText(1,"Zuruck",true);
      if(ispressed()){
        mode=0;
        clearDisplay();
        return;
      }
      break;
    }



  }

}

/**
* showtext
* Shows a scolling text on the display
*   -receives commands over bt
*   -variable scroll speed
*   - narrow or wide letters
*
**/
void showtext(){
  btsend("showtext");
  int speed=2;
  boolean narrow=false;
  while(true){
    int slen, offset;
    int txtlen= message.length()*(4+!narrow);
    if( txtlen <=32){
      //do not scroll if message fits on screen
      offset=(32-txtlen)/2;
      slen=offset;
      Serial.println(offset);
    }else{
      //Scroll area
      offset=28;
      slen = offset-28-txtlen;
    }
    for(int scroll = offset;scroll>=slen;scroll=scroll-speed){
      String recv = btreceive();
      //Cancel
      if(recv.equals("!x") || ispressed()){
        mode=0;
        return;
      }
      //bluetooth receive
      if(recv.equals("!-")){
        speed--;
        btsend("ok:speed-");
      }else if(recv.equals("!+")){
        speed++;
        btsend("ok:speed+");
      }else if(recv.equals("!w")){
        narrow=false;
        btsend("ok:wide");
      }else if(recv.equals("!n")){
        narrow=true;
        btsend("ok:narrow");
      }else if(recv.equals("!h")){
        btsend("####Text Help####\na-z0-9=text\n!-=Speed--\n!+=Speed++\n!w=WideLetters\n!n=NarrowLetters\n!h=help\n!x=exit");
      }else if(!recv.equals("")){
        btsend("ok:"+recv);
        message=recv.substring(0,recv.length()-1);
        break;
      }

      paintText(scroll,message, narrow);
    }
  }
}

void modeloop(){

  //alarm
  if(alarm){
    dtime = clock.getDateTime();
    if(alarmhour == dtime.hour && alarmmin <= dtime.minute){
      alarm=false;
      Serial.println("Alarm");
      buzzer();
    }
  }


  if(mode==1){
    delay(300);
    showtime();
  }
  if(mode==2){
    timer();
  }
  if(mode==6){
    menuOptions();
  }
  if(mode==7){


  }
  if(mode==3){
    stopmode();
  }
  if(mode==4){
    if(alarm){
      for(int x=1;x>-20;x--){
        paintText(x,"Alarm geloescht",true);
        alarm=false;
        setvalue(0);
      }
    }else{
      for(int x=1;x>-20;x--){
        paintText(x,"Alarm in 1h",true);
      }
      dtime = clock.getDateTime();
      alarmhour=dtime.hour+1;
      alarmmin=dtime.minute;
      alarm=true;
      //go to clock
      mode=1;
      setvalue(2);
      clearDisplay();
    }
  }
  if(mode==5){
    showtext();
  }

}

void showtime(){
  dtime = clock.getDateTime();
  // Serial.print(F("Raw Date data: "));
  //  Serial.print(dt.year);   Serial.print("-");
  //  Serial.print(dt.month);  Serial.print("-");
  //  Serial.print(dt.day);    Serial.print(" ");
  // Serial.print(dt.hour);   Serial.print(":");
  // Serial.print(dt.minute); Serial.print(":");
  // Serial.println(dt.second);
//setProgressbar(0);
  paintDigit(0,dtime.hour/10);
  paintDigit(1,dtime.hour%10);
  paintDigit(2,dtime.minute/10);
  paintDigit(3,dtime.minute%10);


  //blinking dot
  if(dtime.second%2==0){
    setColumn(16,B00000000);
  }else{
    setColumn(16,B00100100);
  }

  if(alarm){
    setColumn7(0, 7, B0000011);
  }

  if(bar){
    setProgressbar(dtime.second*100/60);
  }

}

void stopmode(){
  int M=0;
  int m=0;
  int S=0;
  int s=0;

  while(true){
    int currMillis = millis();
    paintDigit(0,M);
    paintDigit(1,m);
    paintDigit(2,S);
    paintDigit(3,s);

    s++;
    if(s == 10){
      s=0;
      S++;
    }
    if(S == 6){
      S=0;
      m++;
    }
    if(m == 10){
      m=0;
      M++;
    }
    if(M == 9 && m==9){
      return;
    }
    if(s%2==0){
      setColumn(16,B00000000);
    }else{
      setColumn(16,B00100100);
    }
    if(ispressed()){
      Serial.println("cancel stop clock");
      mode=-1;
      return;
    }
    int delaynr = (millis()-currMillis);
    delay(1000-delaynr);

  }


}


void timer(){
    int nr[4];
  eingabe(nr,3);
  if(nr[0]==0){
    paintDigit(0,-16);
  }
  boolean init=true;

  for(int x=nr[1]; x>=0; x--){
               paintDigit(1,x);
  for(int x=5; x>=0; x--){
      if(init){
          x=nr[2];
      }
      paintDigit(2,x);
  for(int i=9; i>=0; i--){

      if(init){
        i=nr[3];
        init=false;
      }
      paintDigit(3,i);
      for(int z=0;z<4;z++){
                  delay(250);
                //  char customKey = customKeypad.getKey();
                  if (ispressed()){
                    mode=-1;
                    return;
                  }
       }

  }
  }
  }
 buzzer();
}


void eingabe(int nr[],int digits){
      //Paint ????
  switch(digits){
    case 4:
        paintDigit(0,15);
    case 3:
        paintDigit(1,15);
        setColumn(16,B00100100);
    case 2:
        paintDigit(2,15);
    case 1:
        paintDigit(3,15);
  }


Serial.print("Wait for input\n");
  for(int v=4-digits;v<4;v){
    int customKey = readNumericValue()%10;
    paintDigit(v,customKey);

    if(ispressed()){
        nr[v]=customKey;
        Serial.println(customKey);
        v++;
      }
  }
}

void buzzer(){
  ispressed();

  while(!ispressed()){
    paintText(1,"Alarm");
    Serial.println("Buzzer");
    analogWrite(13, 100);   // turn the LED on (HIGH is the voltage level)
    delay(500);                       // wait for a second
    digitalWrite(13, LOW);
    delay(1500);
    paintText(8,"Alarm");
    analogWrite(13, 200);
    delay(500);
    digitalWrite(13, LOW);
  }
}
