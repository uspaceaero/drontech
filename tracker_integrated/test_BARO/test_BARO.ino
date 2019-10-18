#include<stdio.h>
#include<string.h>
#include <Wire.h>
#include "i2c.h"
#include "i2c_BMP280.h"
#define DEBUG true

// http://arduiniana.org/libraries/tinygpsplus/

BMP280 bmp280;

String uav_id = "SPX-BARO";

int i = 1, c = 0;

float meters, metersold, temperature, pressure, correction = 0;
 
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

  Serial1.println("AT+GPS=1");
  delay(1000);

  Serial1.println("AT+GPSRD=0");
  delay(1000);

  if (bmp280.initialize())
      Serial.println("BMP280 sensor found");
  else
  {
      Serial.println("BMP280 sensor missing");
      //while (1) {}
  }
  bmp280.setEnabled(0);
  bmp280.triggerMeasurement();
  
  tcpConnect();

}

void loop()
{
  
  bmp280.awaitMeasurement();
  bmp280.getTemperature(temperature);
  bmp280.getPressure(pressure);
  bmp280.getAltitude(meters);
  if (c == 0)
  {
    correction = 0 - meters;
    c++;
  }
  int height = meters + correction;  
  metersold = (metersold * 10 + meters)/11;
  bmp280.triggerMeasurement();
  
  String hPa = String(pressure/100,DEC);
  hPa = hPa.substring(0,hPa.indexOf('.') + 3);

  String temp = String(temperature,DEC);
  temp = temp.substring(0,temp.indexOf('.') + 2);
  
  String frame = String(i,DEC) + " " + uav_id + " " + height + " " + hPa + " " + temp;
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

int WAIT = 4500;

void tcpConnect()
{ 
  // sendData("",1000,DEBUG);
  
sendData("AT+CREG?",WAIT,DEBUG);

//Set APN
sendData("AT+CSTT=\"virgin-internet\",\"\",\"\"",WAIT,DEBUG);

//Attache to network
sendData("AT+CGATT=1",WAIT,DEBUG);

sendData("AT+CGDCONT=1,\"IP\",\"APN\"",WAIT,DEBUG);

sendData("AT+CGACT=1",WAIT,DEBUG);

//Get the local IP address
//sendData("AT+CIFSR",WAIT,DEBUG);

sendData("AT+CIPSTATUS",WAIT,DEBUG);

//Connect to TCP server
sendData("AT+CIPSTART=\"TCP\",\"IP\",PORT",WAIT,DEBUG);
}
