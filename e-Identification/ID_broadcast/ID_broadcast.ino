#include <Arduino.h>

#include <ESP8266WiFi.h>

String uav_id = "SPX-DTECH";

extern "C" {
#include "user_interface.h"
}

int c = 0;
int chr = 0;
String text[1] = {uav_id};
byte channel;

// Beacon Packet buffer
uint8_t packet[128] = {         0x80, 0x00, 0x00, 0x00,
                        /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                        /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                        /*22*/  0xc0, 0x6c,
                        /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00,
                        /*32*/  0x64, 0x00,
                        /*34*/  0x01, 0x04,
                        /* SSID */
                        /*36*/  0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00,
                        /*70*/ 0x01, 0x08, 0x82, 0x84,
                        /*74*/ 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03, 0x01,
                        /*82*/  0x04
                      };

void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
  Serial.begin(115200);
}

void loop() {
  chr = 0;
  if (c==1) { // <- Change this for modified text[] size.
  c = 0;
  }
  //Serial.println("start");
  //Serial.println(text.length());
  // Randomize channel //

  channel = random(1, 12);
  wifi_set_channel(channel);

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  
    //int pcount = 42; //Reset packet "pointer" to the first char of the SSID
    int pcount = 38; //Reset packet "pointer" to the first char of the SSID
    
    // Fill the packets for SSID-Name with dots
    //for (int i = 41; i <= 52; i++) {
    //  packet[i] = ph[0];
    //}

    //Serial.println(c);
    //tmp = String(c+10); //convert int to string

    // String operation to order the SSID List
    //packet[38] = ph[1]; // symbol: '
    //packet[39] = tmp[0];
    //packet[40] = tmp[1];
    //packet[41] = ph[3]; // symbol: -
    
    // For every word
    for(int i=0;i<text[c].length();i++){
        packet[pcount] = text[c][i];
        pcount++;
      }

    wifi_send_pkt_freedom(packet, 83, 0);
    wifi_send_pkt_freedom(packet, 83, 0);
    wifi_send_pkt_freedom(packet, 83, 0);
    wifi_send_pkt_freedom(packet, 83, 0);
    
    c++; // number printed before SSID name to display the right order
  
  delay(1);
}
