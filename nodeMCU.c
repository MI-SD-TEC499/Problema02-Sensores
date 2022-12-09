#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <iostream>

#define analogPin A0 // pinagem do potenciômetro

const char* ssid = "INTELBRAS"; //nome da rede utilizada
const char* password = "Pbl-Sistemas-Digitais"; //senha da rede utilizada

int poten = 0; // variável para armazenar o valor lido do potenciômetro
int temp = 27; //variável armazenando um valor para simular o sensor de temperatura
int umid = 50; //variável armazenando um valor para simular o sensor de umidade

byte comando;
byte resposta;

void wifi_Setup(){
  Serial.begin(115200); //inicia a comunicação serial

  WiFi.mode(WIFI_STA); //inicia o modo de conexão
  WiFi.begin(ssid, password); //inicia a conexão com a rede
  while (WiFi.waitForConnectResult() != WL_CONNECTED) { //espera a conexão
    delay(5000); 
    ESP.restart(); //reinicia o ESP
  }
  ArduinoOTA.setHostname("ESP-10.0.0.109"); //define o nome do host

  ArduinoOTA.begin(); //inicia o OTA
}

void setup() {

  pinMode(D1,INPUT); //pinagem do botão 1
  pinMode(D2,INPUT); //pinagem do botão 2
  pinMode(LED_BUILTIN, OUTPUT); //pinagem do led da nodeMCU
  
  wifi_Setup();

  Serial.begin(9600);
}



void loop() {
  ArduinoOTA.handle(); //verifica se há atualizações

  if(digitalRead(D1)==0){ //verifica se estar ocorrendo modificação no botão D1
    temp = temp + 1; //incrementando o valor da temperatura
    delay(500);
  }
  if(digitalRead(D2)==0){ //verifica se estar ocorrendo modificação no botão D2
    umid = umid + 1; //incrementando o valor da umidade
    delay(500);
  }

  if (Serial.available()>0){ //caso tenha dados sendo recebidos
      comando = Serial.read(); //leitura do bit recebido
      delay(500); 

   }
   
   switch (comando){ 
      case 0x01: //caso solicite o valor da umidade
         resposta = 0x03;
         Serial.write(resposta); //enviando o vetor de resposta para a raspberry
         Serial.print(umid);
         delay(500); 

      break;
      case 0x02: //caso solicite o valor da temperatura
         resposta = 0x02;   
         Serial.write(resposta); //enviando o vetor de resposta para a raspberry
         Serial.print(temp);
         delay(500); 

      break;
      case 0x03: //caso solicite a situação da NodeMCU
         resposta = 0x00;
         Serial.write(resposta); //enviando o vetor de resposta para a raspberry
         delay(500); 

      break;
      case 0x04: //caso solicite o valor do potênciometro
         poten = analogRead(analogPin); //leitura do valor do potênciometro
         resposta = 0x01;
         Serial.write(resposta); //enviando o vetor de resposta para a raspberry
         Serial.print(poten);
         delay(500); 

      break; 
      case 0x05: //caso solicite para desligar a led da NodeMCU
         resposta = 0x04;
         digitalWrite(LED_BUILTIN, HIGH); //desligando a led (funcionamento em lógica inversa)
         Serial.write(resposta);
         delay(500); 

      break;
      case 0x06: //caso solicite para ligar a led da NodeMCU
         resposta = 0x05;
         digitalWrite(LED_BUILTIN, LOW); //ligando a led (funcionamento em lógica inversa)
         Serial.write(resposta);
         delay(500); 

      break;

   }
}  
