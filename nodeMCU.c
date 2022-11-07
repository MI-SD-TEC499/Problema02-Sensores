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
int temp = 27; //variável armazenando um valor para simular o sensor de temperatura
int umid = 50; //variável armazenando um valor para simular o sensor de umidade
char num_char[10 + sizeof(char)];
int situacao[8]; //vetor armazenador do vetor enviado pela raspberry
int i = 0; //variável auxiliar para for

void setup() {
  char resposta[8]; //vetor de resposta que será enviada para a raspberry
  for (int j = 0; j < 8; j++) { //inicializando o vetor com '0'
    resposta[j] = '0';
  }

  pinMode(D1,INPUT); //pinagem do botão 1
  pinMode(D2,OUTPUT); //pinagem do botão 2
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



void loop() {
  ArduinoOTA.handle(); //verifica se há atualizações


  if(digitalRead(D1)==1){ //verifica se estar ocorrendo modificação no botão D1
    temp = temp + 1; //incrementando o valor da temperatura
  }
  if(digitalRead(D2)==1){ //verifica se estar ocorrendo modificação no botão D2
    umid = umid + 1; //incrementando o valor da umidade
  }

  if (Serial.available()>0){ //caso tenha dados sendo recebidos
      
      situacao[i] = Serial.read(); //leitura do bit recebido
      delay(2000); 

      if((situacao[i] != 0){ //caso seja diferente de '0' é o valor da instrução solicidada
        int menu = situacao[i]; //variável auxiliar
        switch (menu){ 
            case 1: //caso solicite o valor da umidade
               sprintf(num_char, "%d", umid); //transformando de int para um vetor de char
               //armazenando o valor no final do vetor de resposta
               resposta[6] = num_char[0]; 
               resposta[7] = num_char[1];
               Serial.write(resposta); //enviando o vetor de resposta para a raspberry
               delay(2000);
            break;
            case 2: //caso solicite o valor da temperatura
               sprintf(num_char, "%d", temp); //transformando de int para um vetor de char
               //armazenando o valor no final do vetor de resposta
               resposta[6] = num_char[0];
               resposta[7] = num_char[1];
               Serial.write(resposta); //enviando o vetor de resposta para a raspberry
               delay(2000);
            break;
            case 3: //caso solicite a situação da NodeMCU
               Serial.write(resposta); //enviando o vetor de resposta para a raspberry
               delay(2000);
            break;
            case 4: //caso solicite o valor do potênciometro
               adcValue = analogRead(analogPin); //leitura do valor do potênciometro
               sprintf(num_char, "%d", adcValue); //transformando de int para um vetor de char
               //armazenando o valor no final do vetor de resposta
               resposta[6] = num_char[0];
               resposta[7] = num_char[1];
               Serial.write(resposta); //enviando o vetor de resposta para a raspberry
               delay(2000);
            break; 
            case 5: //caso solicite para desligar a led da NodeMCU
               digitalWrite(LED_BUILTIN, HIGH); //desligando a led (funcionamento em lógica inversa)
               delay(2000);
            break;
            case 6: //caso solicite para ligar a led da NodeMCU
               digitalWrite(LED_BUILTIN, LOW); //ligando a led (funcionamento em lógica inversa)
               delay(2000);
            break;
            default:
            break;
        }
      }  
      i = i + 1; //percorrendo o vetor recebido como instruç
   }
}
