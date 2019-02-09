#include <Arduino.h>
#include "MyDisplay.h"
#include "rotaryencoder.h"
#include "SmartClock.h"
//String my = "Menu";


#define rxPin 11   //virtueller RX-Pin des Moduls zur Kommunikation mit dem HC-05
#define txPin 12 //virtueller TX-Pin des Moduls zur Kommunikation mit dem HC-05

//String Name="Bluething";
String Text="";    //In diesem String werden Texte zwischengespeichert
char Zeichen;      //In dieser Variable werden Zeichen zwischengespeichert
//Initialisieren der virtuellen seriellen Schnittstelle
#include <SoftwareSerial.h>
SoftwareSerial HC05(txPin, rxPin);

void setup() {
  Serial.begin(9600);
  Serial.println(F("Setup"));
  initLed();
  Serial.println(F("Led done"));
  initEncoder();
  Serial.println(F("Encoder done"));
  // put your setup code here, to run once:

  HC05.begin(38400);
  Serial.println(F("Initialize DS3231"));
  setupClock();
  Serial.println(F("Initialize DS3231-2"));
}

void btsend(String msg){
  HC05.write(msg.c_str());
  HC05.write('\n');
}

String btreceive(){
  //String Text;
    //Empfange zeichen vom HC-05 Modul
  Text="";
  while(HC05.available() > 0){
    Serial.print(".");             // So lange etwas empfangen wird, durchlaufe die Schleife.
     Zeichen = HC05.read();                  // Speichere das empfangene Zeichen in der Variablen "Zeichen".
     if (Zeichen == '\n' || Zeichen =='?') {                  // War das letzte Zeichen ein NL (NewLine)?
       Serial.print("bt:");                 // Sende den empfangenen Text an das Modul (samt "\n")
       Serial.println(Text);
       //HC05.write("ok T:150\n");
       return Text;
     }else{
       Text.concat(Zeichen);                   // Füge das Zeichen an den String an, damit wir den kompletten Text erhalten.
     }
  }
return "";

}


void loop() {
  // put your main code here, to run repeatedly:
  //
  //Serial.println("Loop");
  //delay(100);
  if(menuSelect()){
    modeloop();
  }


  if(Serial.available() > 0){
    String txt = Serial.readString();
    Serial.println("Recv:"+txt);
    HC05.write(txt.c_str());
  //  HC05.write('\n');
  }

}
