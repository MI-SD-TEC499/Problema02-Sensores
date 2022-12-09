# Problema 02 - Sensores

### Integrantes: Daniel Costa, Diego Silva e Thiago Menezes

## 1. Introdução 

Com o avanço da tecnologia, inclusive a de construção de transistores, tem sido possibilitado cada vez mais a construção de computadores com menor tamanho, melhor desempenho, incluindo energético, e menor custo. Sistemas embarcados em microcontroladores são utilizados para diversas tarefas de automação e na chamada *Internet of Things* (IoT).

O problema apresentado a seguir consistiu no desenvolvimento de um sistema que integra dois *Single Board Computers* (SBC), sendo uma *Raspberry Pi* e  uma ESP8266 (NodeMCU), para realizar a medida de sensores de umidade e temperatura e exibição desses dados em um display LCD. A comunicação entre esses computadores se dá através da *Universal Asynchronous Receiver / Transmitter* (UART).

## 2. Ambiente e Ferramentas

O problema foi desenvolvido para funcionar em uma Raspberry Pi Zero, utilizando como periférico de saída um display LCD Hitachi HD44780U (LCD-II) de 16x2. Essa Raspberry possui arquitetura ARMv6 de 32bits.

O projeto também utiliza uma ESP8266 (NodeMCU) conectada a sensores de umidade e temperatura analógicos e digitais. Os sensores digitais são emulados por push buttons e o analógico por um potenciômetro.

Para comunicação com os periféricos, tanto da NodeMCU quanto da Raspberry PI, foi utilizada a biblioteca WiringPi, enquanto que a utilização da UART ocorreu através dos comandos de controle e utilização oferecidos pela biblioteca TermIOs. Todos os códigos utilizados foram produzidos em linguagem C. Também foi necessária a utilização da bibliotca ArduinoOTA para possibilitar o carregamento de código na NodeMCU via Wifi.

Para edição, compilação e upload de código para a NodeMCU foi utilizada a ArduinoIDE, enquanto para a Raspberry Pi foi necessário apenas o editor Nano.

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
void print_display(uint8_t *resposta){
    int lcd; //variável para manipulação do lcd
    wiringPiSetup(); //configuração do display        
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0); //pinando o display
    int code = resposta[0]; //variável auxiliar 
    switch(code){
	case 0x00: //caso a comunicação com a NodeMCU esteja funcionando corretamente
		lcdPrintf(lcd,"NODEMCU: ON");    
	break;
	case 0x09: //caso a comunicação com a NodeMCU não esteja funcionando corretamente
        	lcdPrintf(lcd,"NODEMCU: OFF");    
        break;
        case 0x01: //exibição do valor obtido do potênciomento
        	lcdPrintf(lcd,"Potenciometro: %d", resposta[1]);
        break;
        case 0x02: //exibição do valor obtido do sensor de temperatura
        	lcdPrintf(lcd,"Temperatura: %d C", resposta[1]);
        break;
        case 0x03: //exibição do valor obtido do sensor de umidade
            lcdPrintf(lcd,"Umidade: %d %%", resposta[1]);
        break;
        case 0x04: //exibindo quando a led da NodeMCU fica desativada
        	lcdPrintf(lcd,"LED: OFF");
        break;
        case 0x05: //exibindo quando a led da NodeMCU fica ativada
        	lcdPrintf(lcd,"LED: ON");
	break;
        default: //caso aconteça algum erro
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

## 4. Executando o projeto

### 4.1 NodeMCU

Com o ambiente preparado, não se deve esquecer de inserir alguns trechos de código no projeto caso vá ser utilizado o upload via wifi, cada trecho deve ser colocado em sua respectiva parte do loop/setup ou fora deles. Esses trechos da ArduinoOTA ficarão assim:

```c
const char* ssid = "SSID-DA-REDE-A-SER-UTILIZADA";
const char* password = "SENHA-DA-REDE-A-SER-UTILIZADA"; 

void setup() {
  Serial.begin(9600); //inicia a comunicação serial

  WiFi.mode(WIFI_STA); //inicia o modo de conexão
  WiFi.begin(ssid, password); //inicia a conexão com a rede
  while (WiFi.waitForConnectResult() != WL_CONNECTED) { //espera a conexão
    delay(5000); 
    ESP.restart(); //reinicia o ESP
  }
  ArduinoOTA.setHostname("HOST-DA-PLACA"); //define o nome do host

  ArduinoOTA.begin(); //inicia o OTA
}

void loop() {
  ArduinoOTA.handle(); //verifica se há atualizações
}
```

Com isso configurado, e com a porta selecionada na interface da ArduinoIDE, basta realizar a verificação e upload do código na placa.

### 4.2 Raspberry PI

O processo na raspberry é mais simples, bastando acessar a placa via SSH (ou no método que preferir), carregar os arquivos do projeto, compilar, linkar e executar, sendo esses últimos passos possibilitados pela utilização de um makefile. 
Para esse caso, basta acessar o diretório raíz do projeto através do terminal e executar o comando `make all`.

Com todos os componente em execução, já é possível realizar a utilização do projeto e testá-lo.

Através da raspberry iniciamos a comunicação UART, para isso utilizamos duas bibliotecas, “termios.h”  e “unistd.h”. 
Na função “open_serial_port” recebemos o file descriptor referente ao arquivo “ttyS0” que está associado à UART da placa:

```c
static void open_serial_port(void) {
    g_fd = file_open_and_get_descriptor(SERIAL_PORT_PATH);
    if(g_fd < 0) {
        printf("Something went wrong while opening the port...\r\n");
        exit(EXIT_FAILURE);
    }
}
```
em seguida no método “configure_serial_port” , armazenamos os atributos da UART e definimos a Baud Rate, por fim chamamos a função “cfmakeraw” que realiza as configurações finais.

```c
static void configure_serial_port(void) {
    if(tcgetattr(g_fd, &g_tty)) {
        printf("Something went wrong while getting port attributes...\r\n");
        exit(EXIT_FAILURE);
    }

	//115200
    cfsetispeed(&g_tty,B9600);
    cfsetospeed(&g_tty,B9600);

    cfmakeraw(&g_tty);

    if(tcsetattr(g_fd,TCSANOW,&g_tty)) {
        printf("Something went wrong while setting port attributes...\r\n");
        exit(EXIT_FAILURE);
    }
}
```
com a UART configurada, podemos iniciar o envio de dados utilizando a função “write” onde informamos o conteúdo e o tamanho da mensagem, a função read tem uma estrutura parecida, mas é usada para a leitura de dados.

```c
static int file_write_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return write(fd,buff,len_buff);
}

static int file_read_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return read(fd,buff,len_buff);
}
```


## 5. Testes de Funcionamento

Para testar a exibição dos dados no display, foram realizados alguns testes do projeto. Esses testes objetivaram verificar tanto a corretude do funcionamento da UART quanto das operações desejadas ao se receber um determinado dado.

O projeto inicialmente apresenta um menu de opções, através do qual podemos realizar as operações desejadas, esse menu está apresentado na imagem seguinte.

![image](https://user-images.githubusercontent.com/38412142/203539817-56de0939-3f37-471f-a527-5240b47d8f22.png)

Cada uma das opções, espera um resposta apresentada no display lcd.

- Verificação de modo

Ao selecionar a opção 1 do menu, o resultado esperado é que seja apresentado o modo que a NodeMCU está no momento, podendo esse modo ser ligado ou desligado. Ao selecioná-la com ela ligada, o resultado seguinte é exibido:

![nodemcu on](https://user-images.githubusercontent.com/38412142/206601567-f4f230db-3d36-4291-b037-ee5021c8cdc5.jpeg)

Ao selecionar essa opção com a NodeMCU desligada, é esperado a exibição do modo OFF, como obtido na imagem a seguir:

![modo off](https://user-images.githubusercontent.com/38412142/200310953-5a2eb3f0-bdb1-4f5b-966a-e54e13041808.jpeg)

- Captação analógica

Ao selecionar a opção 2 do menu, é esperado o comportamento de exibição do valor captado pelo potenciômetro que emula um sensor analógico. A imagem a seguir mostra a saída obtida, compatível com o valor obtido do potenciômetro

![potenciometro](https://user-images.githubusercontent.com/38412142/206601603-e6331216-a172-43b8-875e-808521c1af69.jpeg)

- Captação de umidade e temperatura digital

Ao selecionar a opções 3, o display deve exibir o valor obtido através da leitura do sensor de temperatura digital, emulado por um push-button. Esse comportamento é exibido na imagem a seguir.

![temperatura](https://user-images.githubusercontent.com/38412142/206601618-6f369889-6b8d-492d-af6d-c39b2a75429c.jpeg)

A opção 4, pretende mostrar o valor obtido pelo outro emulador de sensor digital, que capta a umidade. Também é possível observar na imagem o pleno funcionamento da solicitação dessa opção.

![umidade](https://user-images.githubusercontent.com/38412142/206601651-f4c4cfc2-4df1-4284-92c0-d21332895be0.jpeg)

- Controle de LED

Há no menu também o controle de leds, as opções 5 e 6 realizam as operações de ligar e desligar o led, respectivamente. Esse comportamento foi implementado de forma adequada e é possível atestar isso através do resultado exibido nas imagens seguintes.

Exibição do display ao selecionar a opções 5:

![led on](https://user-images.githubusercontent.com/38412142/206601684-69327f34-c09e-4196-a33d-dc0861cb4f55.jpeg)

Exibição do display ao selecionar a opção 6:

![led off](https://user-images.githubusercontent.com/38412142/206601689-8ce28744-f5c8-4346-b834-7bef723101a7.jpeg)

- Erro

Para situações não mapeadas, ou caso ocorra um erro em uma das operações, é esperado que seja exibido no display LCD uma mensagem contendo "ERRO", o que é possível observar na imagem seguinte, no caso em que foi inserido uma opção inválida do menu.

[ ADICIONAR IMAGEM ERRO ]

Do lado da NodeMCU, para testar o funcionamento dos sensores, utilizamos eles para controlar o LED. Por exemplo, com os sensores digitais, caso ativados eles acendem o LED da ESP8266, para o teste do sensor analógico, ele é utilizado para controlar a intensidade do brilho desse led.

## 5. Referências

- Documentação da Biblioteca WiringPi, para controle de periféricos da Raspberry através da linguagem C:
 
[WiringPi](http://wiringpi.com/reference/)


