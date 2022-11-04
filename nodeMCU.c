#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <iostream>

#define analogPin A0 // pinagem do potenciômetro

const char* ssid = "INTELBRAS"; //nome da rede utilizada
const char* password = "Pbl-Sistemas-Digitais"; //senha da rede utilizada

int adcValue = 0; // variável para armazenar o valor lido do potenciômetro
int adcAux = 0; // variável auxiliar para armazenar o valor lido do potenciômetro

void setup() {
  //pinMode(D1,INPUT); //pinagem do botão 1
  //pinMode(D2,OUTPUT); //pinagem do botão 2
  pinMode(LED_BUILTIN, OUTPUT); //pinagem do led da nodeMCU
  Serial.begin(9600); //inicia a comunicação serial

  WiFi.mode(WIFI_STA); //inicia o modo de conexão
  WiFi.begin(ssid, password); //inicia a conexão com a rede
  while (WiFi.waitForConnectResult() != WL_CONNECTED) { //espera a conexão
    delay(5000); 
    ESP.restart(); //reinicia o ESP
  }
  ArduinoOTA.setHostname("ESP-10.0.0.109"); //define o nome do host

  ArduinoOTA.begin(); //inicia o OTA
}
int situacao[8];
int i = 0;
char resposta[8];
for (int j = 0; j < 8; j++) {
  resposta[j] = '0';
}
int temp = 27;
int umid = 50;
char num_char[10 + sizeof(char)];


void loop() {
  ArduinoOTA.handle(); //verifica se há atualizações


  if(digitalRead(D1)==1){
    temp = temp + 1;
  }
  if(digitalRead(D2)==1){
    umid = umid + 1;
  }

  if (Serial.available()>0){
      digitalWrite(LED_BUILTIN, HIGH);
      delay(2000); 
      
      situacao[i] = Serial.read();
      delay(2000);

      if((situacao[i] != 0){
        int menu = situacao[i];
        switch (menu){
          case 3:
            Serial.write(resposta);
            delay(2000);
          break;
          case 4:
            adcValue = analogRead(analogPin); //leitura do valor do potenciômetro
            sprintf(num_char, "%d", adcValue);
            resposta[6] = num_char[0];
            resposta[7] = num_char[1];
            Serial.write(resposta);
            delay(2000);
          break; 
          case 2:
            sprintf(num_char, "%d", temp);
            resposta[6] = num_char[0];
            resposta[7] = num_char[1];
            Serial.write(resposta);
            delay(2000);
          break;
          case 1:
            sprintf(num_char, "%d", umid);
            resposta[6] = num_char[0];
            resposta[7] = num_char[1];
            Serial.write(resposta);
            delay(2000);
          break;
          case 6:
            digitalWrite(LED_BUILTIN, LOW); 
            delay(2000);
          break;
          case 5:
            digitalWrite(LED_BUILTIN, HIGH); 
            delay(2000);
          default:
          break;
        }
        //Serial.write(situacao[i]);
        //delay(2000);
      }  
      i = i + 1;
   }
}
