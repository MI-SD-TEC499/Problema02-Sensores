# Problema 02 - Sensores

### Integrantes: Daniel Costa, Diego Silva e Matheus Menezes

## 1. Introdução 

## 2. Ambiente e Ferramentas

O problema foi desenvolvido para funcionar em uma Raspberry Pi Zero (ou Orange PI PC Plus), utilizando como periférico de saída um display LCD Hitachi HD44780U (LCD-II) de 16x2. Essa Raspberry possui arquitetura ARMv6 de 32bits.

O projeto também utiliza uma ESP8266 (NodeMCU) conectada a sensores de umidade e temperatura analógicos e digitais. Os sensores digitais são emulados por push buttons e o analógico por um potenciômetro.

Para comunicação com os periféricos e para a utilização da UART, foi utilizada a biblioteca WiringPi, e códigos produzidos em linguagem C.

Para edição, compilação e upload de código para a NodeMCU foi utilizada a ArduinoIDE, enquanto para a Orange Pi foi necessário apenas o Nano.

## 3. Desenvolvimento

## 4. Testes de Funcionamento

gcc teste.c start.o -o start
