#include<stdio.h>
#include<string.h>
#include "TinyGPS++.h"
#define DEBUG true

TinyGPSPlus nmeaPlus;
float lat, lon;
unsigned long time, date;

String uav_id = "SPX-DRONTECH";

int i = 1;
 
void setup()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8,OUTPUT);
  digitalWrite(5, HIGH); 
  digitalWrite(4, LOW); 
  digitalWrite(8, LOW);
  
  delay(2000);
  Serial.println("Test begin!!");
  
  digitalWrite(4, HIGH); 
  delay(3000);       
  digitalWrite(4, LOW);
  Serial.println("A7 Power OFF!");
  delay(3000);
  
  digitalWrite(8, HIGH); 
  delay(3000);       
  digitalWrite(8, LOW);
  Serial.println("A7 Power ON!");
  delay(1000);

  Serial1.println("AT+GPS=0");
  delay(1000);

  Serial1.println("AT+GPSRD=0");
  delay(1000);
  
  tcpConnect();

}

void loop()
{
   getNMEA("AT+GPSRD=1",1000,DEBUG);
   date = nmeaPlus.date.value();
   time = nmeaPlus.time.value();
   lat = nmeaPlus.location.lat();
   lon = nmeaPlus.location.lng();

   String tim = String(time,DEC);
   String la = String(lat,DEC).substring(0,9);
   String lo = String(lon,DEC);
   if (lo.length()==14)
    lo = lo.substring(0,10);
   else {
    lo = "0" + lo.substring(0,9);
   }
  
  String frame = String(i,DEC) + " " + uav_id + " " + tim.substring(0,6) + " " + la + " " + lo;
  frame = frame + "\r\n\x1A";
  i++;

  sendTcp(frame,1500);
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();   
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {       
        char c = Serial1.read(); 
        response+=c;
      }  
    }    
    if(debug)
    {
      Serial.println(response);
    }    
    return response;
}

void sendTcp(String data, const int timeout)
{   
    Serial1.println("AT+CIPSEND");
    delay(100);
    Serial1.println(data);
    Serial.println(data);
    long int time = millis();   
    while( (time+timeout) > millis()) {}
}

void tcpConnect()
{
sendData("AT+CREG?",1000,DEBUG);

//Set APN
sendData("AT+CSTT=\"APN\",\"\",\"\"",1000,DEBUG);

//Attache to network
sendData("AT+CGATT=1",4000,DEBUG);

sendData("AT+CGDCONT=1",4000,DEBUG);

sendData("AT+CIPSTATUS",4000,DEBUG);

//Bring up the wireless connection
sendData("AT+CIICR",4000,DEBUG);

//Get the local IP address
sendData("AT+CIFSR",4000,DEBUG);

//Connect to TCP server
sendData("AT+CIPSTART=\"TCP\",\"IP\",PORT",1000,DEBUG);
}

String getNMEA(String command, const int timeout, boolean debug)
{
    String response = "";    
    Serial1.println(command); 
    long int time = millis();   
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {       
        nmeaPlus.encode(Serial1.read());
      }  
    }    
    if(debug)
    {
      Serial.print(response);
    }    
    return response;
}
