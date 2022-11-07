# Problema 02 - Sensores

### Integrantes: Daniel Costa, Diego Silva e Thiago Menezes

## 1. Introdução 

Com o avanço da tecnologia, inclusive a de construção de transistores, tem sido possibilitado cada vez mais a construção de computadores com menor tamanho, melhor desempenho, incluindo energético, e menor custo. Sistemas embarcados em microcontroladores são utilizados para diversas tarefas de automação e na chamada *Internet of Things* (IoT).

O problema apresentado a seguir consistiu no desenvolvimento de um sistema que integra dois *Single Board Computers* (SBC), sendo uma *Raspberry Pi* e  uma ESP8266 (NodeMCU), para realizar a medida de sensores de umidade e temperatura e exibição desses dados em um display LCD. A comunicação entre esses computadores se dá através da *Universal Asynchronous Receiver / Transmitter* (UART).

## 2. Ambiente e Ferramentas

O problema foi desenvolvido para funcionar em uma Raspberry Pi Zero, utilizando como periférico de saída um display LCD Hitachi HD44780U (LCD-II) de 16x2. Essa Raspberry possui arquitetura ARMv6 de 32bits.

O projeto também utiliza uma ESP8266 (NodeMCU) conectada a sensores de umidade e temperatura analógicos e digitais. Os sensores digitais são emulados por push buttons e o analógico por um potenciômetro.

Para comunicação com os periféricos e para a utilização da UART, foi utilizada a biblioteca WiringPi, e códigos produzidos em linguagem C.

Para edição, compilação e upload de código para a NodeMCU foi utilizada a ArduinoIDE, enquanto para a Orange Pi foi necessário apenas o Nano.

## 3. Desenvolvimento

Primeiro será apresentado o fluxo do projeto e em seguida as implementações em cada um dos SBCs.

### 3.1 Fluxo de funcionamento do projeto

De forma simplificada, o projeto consiste no envio de informações via UART da Raspberry Pi para a NodeMCU, que tomara um curso de ação de acordo a informação recebida. O fluxo mais comum consiste em ler a informação vinda dos sensores e enviá-la de volta via UART para a Raspberry Pi que se encarregará de exibi-la no display. Para outros casos, pode não ocorrer a leitura da informações dos sensores, mas todos seguem o fluxo de comunicação de volta e exibição da informação no LCD.

![diagrama2](https://user-images.githubusercontent.com/38412142/199864012-eaaeb39f-32b8-4355-ba21-03b63e2f739f.png)

### 3.2 Raspberry Pi Zero

Na Raspberry Pi foi desenvolvido o código responsável por fazer o envio de códigos via UART para realizar solicitações de serviços para a ESP8266, assim como aguarda a resposta.

```c
file_write_data(g_fd,writeTest,sizeof(writeTest));
sleep(1);
memset(l_buff,0,l_len_buff);
file_read_data(g_fd,writeTest,sizeof(readTest));
```

Seguindo o funcionamento do protocolo UART, os dados são enviados de forma paralela para a UART, que os transmite de forma serial para a UART da ESP8266. 

Também foi utilizada a biblioteca WiringPI para realizar a escrita no display.

```c
void print_display(int num, int value){
    int lcd;
    wiringPiSetup();        
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
    int code = num;
    switch(code){
	    case 0:
	        lcdPrintf(lcd,"NODEMCU: ON");    
	    break;
	    case 9:
        	lcdPrintf(lcd,"NODEMCU: OFF");    
        break;
        case 1:
        	lcdPrintf(lcd,"Potênciometro: %d", value);
        break;
        case 2:
        	lcdPrintf(lcd,"Temperatura: %d ºC", value);
        break;
        case 3:
        	lcdPrintf(lcd,"Umidade: %d %%", value);
        break;
        case 4:
        	lcdPrintf(lcd,"LED: ON");
        break;
        case 5:
        	lcdPrintf(lcd,"LED: OFF");
        default:
        	lcdPrintf(lcd,"ERRO");        
	    break;
    }
}

```

### 3.3 NodeMCU (ESP8266)

Na NodeMCU há dois grandes módulos implementados. A leitura de informação dos sensores analógicos e digital, e assim como na Raspberry, a comunicação UART.

```c
if((situacao[i] != 0){
        int menu = situacao[i];
        switch (menu){
          case 3:
            Serial.write(resposta);
            delay(2000);
```
Inicialmente o sensor digital seria um DHT11, porém, para o problema foram utilizados *push buttons* para simular o comportamento.

Em caso de informações dos sensores, a NodeMCU recebe esses dados paralelamente e os envia para a UART para que os transmita de volta para a UART da Raspberry Pi.

## 4. Testes de Funcionamento

Para testar a exibição dos dados no display, fizemos um *loopback*, ou seja, a Raspberry recebe o mesmo dado que está enviando, com o propósito de verificar tanto a corretude do funcionamento da UART quanto das operações desejadas ao se receber um determinado dado.

Através disso, enviamos as possibilidades de respostas para o display, sendo elas: 
- Verificação de modo;
- Captação de umidade e temperatura digital
- Captação de umidade e temperatura analógica
- Acendimento de LED
- Erro
-  {{ COMPLETAR A LISTA }}

{{ ADICIONAR FOTOS DOS MODOS IMPRESSOS }}

Do lado da NodeMCU, para testar o funcionamento dos sensores, utilizamos eles para controlar o LED. Por exemplo, com os sensores digitais, caso ativados eles acendem o LED da ESP8266, para o teste do sensor analógico, ele é utilizado para controlar a intensidade do brilho desse led.

gcc teste.c start.o -o start

## 5. Referências
