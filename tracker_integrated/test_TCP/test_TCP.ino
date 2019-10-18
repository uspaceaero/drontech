#include<stdio.h>
#include<string.h>
#define DEBUG true

String uav_id = "SPX-DRONTECH";

int i = 0;
 
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
  delay(3000);

//  digitalWrite(8, HIGH); 
//  delay(3000);       
//  digitalWrite(8, LOW);
//  Serial.println("A7 Power ON!");
//  delay(3000);

  Serial1.println("AT+GPS=0");
  delay(1000);

  Serial1.println("AT+GPSRD=0");
  delay(1000);
  
  tcpConnect();
}

void loop()
{
  String frame = String(i,DEC) + " " + uav_id;
  frame = frame + "\r\n\x1A";
  i++;
  
//  Serial1.println("AT+CIPSEND");
//  delay(500);
//  Serial1.println(frame);
//  delay(500);

  sendData("AT+CIPSEND",500,false);
  sendData(frame,500,DEBUG);  
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

int WAIT = 4500;

void tcpConnect()
{ 
  // sendData("",1000,DEBUG);
  
sendData("AT+CREG?",WAIT,DEBUG);

//Set APN
sendData("AT+CSTT=\"APN\",\"\",\"\"",WAIT,DEBUG);

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

//Bring up the wireless connection
//sendData("AT+CIICR",WAIT,DEBUG);
