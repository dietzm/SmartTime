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

void setupClock(){
  clock.begin();
}

boolean menuSelect(){
  //buzzer();

  int val = getvalue()/2%7;
  if(val != mode || mode < 1){
    //Show Menu
    mode=0;
    switch(val){
     case 0:
          paintText(1,"Menu");
          break;
     case 1:
          paintText(1,"Clock");
          break;
     case 2:
          paintText(1,"Timer");
          break;
     case 3:
          paintText(1,"Stopp");
          break;
     case 4:
          paintText(1,"Zocken");
          break;
     case 5:
          paintText(1,"Text");
          break;
     case 6:
           paintText(1,"Options");
           break;
    }
    if(ispressed()){
      mode=val;
      clearDisplay();
    }else{
      return false;
    }
  }
  return true;
}

void menuOptions(){
  ispressed();
  int pos=1;
  while(true){
  if(pos<-8){
      pos=1;
  }
  int val = getvalue()/2%6;
  //Show Menu
    switch(val){
     case 0:
          if(bar){
            paintText(1,"Bar  {");
          }else{
            paintText(1,"Bar  x");
          }
          if(ispressed()){
            bar = !bar;
            continue;
          }
          break;
     case 1:
          paintText(pos--,"Set Time");

          if(ispressed()){
            clearDisplay();
            int nr[4];
            eingabe(nr);
             dtime = clock.getDateTime();
             int hour=nr[0]*10+nr[1];
             int mini=nr[2]*10+nr[3];
            clock.setDateTime(dtime.year,dtime.month,dtime.day,hour,mini,0);
            mode=0;
          }

          break;
     case 2:
          paintText(pos--,"Light");
          if(ispressed()){
            intensity++;
            if(intensity>=9){
              intensity=1;
            }
            setIntensity(intensity);
          }
          break;
     case 3:
          paintText(1,"Opt");
          break;
     case 4:
          paintText(1,"Opt4");
          break;
     case 5:
           paintText(1,"Back");
           if(ispressed()){
             mode=0;
             clearDisplay();
             return;
           }
           break;
    }



}

}
void modeloop(){
  if(mode==1){
    delay(500);
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
        paintText(x,"Alarm disabled");
        alarm=false;
        setvalue(0);
      }
    }else{
      for(int x=1;x>-20;x--){
        paintText(x,"Alarm in 1h");
      }
      dtime = clock.getDateTime();
      alarmhour=dtime.hour+1;
      alarmmin=dtime.minute;
      alarm=true;
      //go to clock
      mode=1;
      setvalue(1);
    }
  }
  if(mode==5){
    for(int scroll = 28;scroll>=-40;scroll=scroll-2){
      paintText(scroll,"Marie");
      //Cancel?
      if(ispressed()){
        mode=0;
        return;
      }
    }
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
    if(bar){
      setProgressbar(dtime.second*100/60);
    }
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
  eingabe(nr);
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


void eingabe(int nr[]){
      //Paint ????
  paintDigit(0,15);
  paintDigit(1,15);
  paintDigit(2,15);
  paintDigit(3,15);
Serial.print("Wait for input\n");
  for(int v=0;v<4;v){
    int customKey = getvalue()%10;
    paintDigit(v,customKey);

      if(customKey > 9 || customKey < 0){
        paintDigit(v,22);
        delay(500);
        paintDigit(v,15);
        continue;
      }
      if(ispressed()){
        nr[v]=customKey;
        Serial.println(customKey);
        v++;

      }

  }
}

void buzzer(){
  Serial.println("Buzzer");
    analogWrite(13, 100);   // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(13, LOW);
  delay(1500);
   analogWrite(13, 200);
  delay(500);
  digitalWrite(13, LOW);
  delay(1500);
  analogWrite(13, 300);  // turn the LED on (HIGH is the voltage level)
  delay(500);                       // wait for a second
  digitalWrite(13, LOW);
  delay(1500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
}
