#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <wiringPi.h>          
#include <lcd.h>         

#define SERIAL_PORT_PATH    "/dev/ttyS0"
#define LCD_RS  6               //Register select pin
#define LCD_E   31              //Enable Pin
#define LCD_D4  26              //Data pin 4
#define LCD_D5  27              //Data pin 5
#define LCD_D6  28              //Data pin 6
#define LCD_D7  29              //Data pin 7

struct termios g_tty;
int g_fd;
uint8_t l_buff[256];
uint32_t l_len_buff = 256;

/*
* Função para exibição de dados no displau LCD
*/
void print_display(int num, int value){
    int lcd; //variável para manipulação do lcd
    wiringPiSetup(); //configuração do display        
    lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0); //pinando o display
    int code = num; //variável auxiliar 
    switch(code){
	    case 0: //caso a comunicação com a NodeMCU esteja funcionando corretamente
	        lcdPrintf(lcd,"NODEMCU: ON");    
	    break;
	    case 9: //caso a comunicação com a NodeMCU não esteja funcionando corretamente
        	lcdPrintf(lcd,"NODEMCU: OFF");    
            break;
            case 1: //exibição do valor obtido do potênciomento
        	lcdPrintf(lcd,"Potenciometro: %d", value);
            break;
            case 2: //exibição do valor obtido do sensor de temperatura
        	lcdPrintf(lcd,"Temperatura: %d ºC", value);
            break;
            case 3: //exibição do valor obtido do sensor de umidade
        	lcdPrintf(lcd,"Umidade: %d %%", value);
            break;
            case 4: //exibindo quando a led da NodeMCU fica ativada
        	lcdPrintf(lcd,"LED: ON");
            break;
            case 5: //exibindo quando a led da NodeMCU fica desativada
        	lcdPrintf(lcd,"LED: OFF");
            default: //caso aconteça algum erro
        	lcdPrintf(lcd,"ERRO");        
	    break;
    }
}


// FILE OPERATION
static int file_open_and_get_descriptor(const char *fname) {
    int fd;

    fd = open(fname, O_RDWR | O_NONBLOCK);
    if(fd < 0) {
        printf("Could not open file %s...%d\r\n",fname,fd);
    }
    return fd;
}

/*
* Função para realizar o envio do vetor de instrução para a NodeMCU
*/
static int file_write_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return write(fd,buff,len_buff);
}

/*
* Função para realizar a leitura do vetor de resposta da NodeMCU
*/
static int file_read_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return read(fd,buff,len_buff);
}

static int file_close(int fd) {
    return close(fd);
}

/*
* Função para iniciar porta serial de comunicação
*/
static void open_serial_port(void) {
    g_fd = file_open_and_get_descriptor(SERIAL_PORT_PATH);
    if(g_fd < 0) {
        printf("Something went wrong while opening the port...\r\n");
        exit(EXIT_FAILURE);
    }
}


/*
* Função para configurar porta serial de comunicação
*/
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

/*
* Função para encerrar comunicação serial
*/
static void close_serial_port(void) {
    file_close(g_fd);
}

/*
* Função para enviar um vetor para a NodeMCU 
*/
uint8_t send_to_node(uint8_t *writeTest){
    uint8_t auxTest[8] = writeTest; //vetor para auxilio na manipulação
    file_write_data(g_fd,auxTest,sizeof(auxTest)); //função para envio do vetor
    sleep(5); //espera para não acontecer erros no envio 
    memset(l_buff,0,l_len_buff);
    file_read_data(g_fd,auxTest,sizeof(auxTest)); //leitura da resposta obtida
    
    return auxTest; //retornando resposta
}

int main(void) {
    printf("Starting the loopback application...\r\n");

    open_serial_port(); //abertura da porta serial para comunicação

    configure_serial_port(); //configuração da porta serial
    
    //variáveis para auxilio do menu
    int aux=0; 
    int menu=0; 
    
    uint8_t writeTest[8]; //vetor que irá armazenar as solicitações 
    uint8_t readTest[8]; //vetor que irá armazenar as respostas
    char *ptr; //ponteiro de char para ser utilizado na função strtol
    long value; //valor do resultado da transformação de vetor para long


    while(aux==0){ //menu para interação com o usuário
        printf("1 -- Solicita a situação atual da NodeMCU\n");
        printf("2 -- Valor de entrada analogica\n");
        printf("3 -- Valor da temperatura\n");
        printf("4 -- Valor da umidade\n");
        printf("5 -- Acender a led\n");
        printf("6 -- Apagar a led\n");
        scanf("%d" , &menu);
        switch(menu){
            case 1: //caso para solicitar a situação da NodeMCU
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 3;

                readTest = send_to_node(writeTest); //leitura do resultado obtido pela comunicação com a NodeMCU
            
                if(readTest[7] == 0){ //caso a comunicação com a NodeMCU esteja funcionando corretamente
                    print_display(0,0); //enviando para o display o que deve ser exibido
                }else if (readTest[7] == 9){ //caso a comunicação com a NodeMCU não esteja funcionando corretamente
                { 
                    print_display(9,0); //enviando para o display o que deve ser exibido
                }
            break;
            case 2: //caso para realizar a leitura do sensor analogico
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 4;

                readTest = send_to_node(writeTest); //leitura do resultado obtido pela comunicação com a NodeMCU
			
                value = strtol(readTest, &ptr, 10); //transformando o vetor de char(string) em um long
                print_display(1,value); //enviando para o display o que deve ser exibido
            break;
            case 3: //caso para realizar a leitura do sensor de temperatura
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU	
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 2;

                readTest = send_to_node(writeTest); //leitura do resultado obtido pela comunicação com a NodeMCU
			
                value = strtol(readTest, &ptr, 10); //transformando o vetor de char(string) em um long
                print_display(2,value); //enviando para o display o que deve ser exibido
            break;
            case 4: //caso para realizar a leitura do sensor de umidade 
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU		
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 1;

                readTest = send_to_node(writeTest); //leitura do resultado obtido pela comunicação com a NodeMCU
			
                value = strtol(readTest, &ptr, 10); //transformando o vetor de char(string) em um long
                print_display(3,value); //enviando para o display o que deve ser exibido
            break;
            case 5: //caso para acender a led da NodeMCU
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU	
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 6;

                send_to_node(writeTest); //enviando a instrução para a NodeMCU
                print_display(4,value); //enviando para o display o que deve ser exibido
            break;
            case 6: //caso para desligar a led da NodeMCU
		//criando array com o bytes que serão interpretados como a instrução desejada na NodeMCU		
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 5;

                send_to_node(writeTest); //enviando a instrução para a NodeMCU
                print_display(5,value); //enviando para o display o que deve ser exibido
            default: //caso o usuário digite um valor que não corresponda aos programados
                printf("Opção invalida\n\n\n");
                aux=0;
            break;
        }
    }
	    
    close_serial_port(); //encerrando a porta serial de comunicação

    return 0; //encerramento do programa
}
