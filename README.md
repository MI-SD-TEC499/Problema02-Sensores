# Problema 02 - Sensores

### Integrantes: Daniel Costa, Diego Silva e Matheus Menezes

## 1. Introdução 

Com o avanço da tecnologia, inclusive a de construção de transistores, tem sido possibilitado cada vez mais a construção de computadores com menor tamanho, melhor desempenho, incluindo energético, e menor custo. Sistemas embarcados em microcontroladores são utilizados para diversas tarefas de automação e na chamada *Internet of Things* (IoT).

O problema apresentado a seguir consistiu no desenvolvimento de um sistema que integra dois *Single Board Computers* (SBC), sendo uma *Raspberry Pi* e  uma ESP8266 (NodeMCU), para realizar a medida de sensores de umidade e temperatura e exibição desses dados em um display LCD. A comunicação entre esses computadores se dá através da *Universal Asynchronous Receiver / Transmitter* (UART).

## 2. Ambiente e Ferramentas

O problema foi desenvolvido para funcionar em uma Raspberry Pi Zero, utilizando como periférico de saída um display LCD Hitachi HD44780U (LCD-II) de 16x2. Essa Raspberry possui arquitetura ARMv6 de 32bits.

O projeto também utiliza uma ESP8266 (NodeMCU) conectada a sensores de umidade e temperatura analógicos e digitais. Os sensores digitais são emulados por push buttons e o analógico por um potenciômetro.

Para comunicação com os periféricos e para a utilização da UART, foi utilizada a biblioteca WiringPi, e códigos produzidos em linguagem C.

Para edição, compilação e upload de código para a NodeMCU foi utilizada a ArduinoIDE, enquanto para a Orange Pi foi necessário apenas o Nano.

## 3. Desenvolvimento

### 3.1 Raspberry Pi Zero

Na Raspberry Pi foi desenvolvido o código responsável por fazer o envio de códigos via UART para realizar solicitações de serviços para a ESP8266.

{{ CÓDIGO DE ENVIO DE MENSAGEM }}

Também foi utilizada a biblioteca WiringPI para realizar a escrita no display.

### 3.2 NodeMCU (ESP8266)

## 4. Testes de Funcionamento

gcc teste.c start.o -o start
