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
   // lcdPrintf(lcd,"valor %d", writeTest[1]);    

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

static int file_write_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return write(fd,buff,len_buff);
}

static int file_read_data(int fd, uint8_t *buff, uint32_t len_buff) {
    return read(fd,buff,len_buff);
}

static int file_close(int fd) {
    return close(fd);
}


static void open_serial_port(void) {
    g_fd = file_open_and_get_descriptor(SERIAL_PORT_PATH);
    if(g_fd < 0) {
        printf("Something went wrong while opening the port...\r\n");
        exit(EXIT_FAILURE);
    }
}

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

static void perform_demo(void) {
   uint8_t l_buff[256];
   uint32_t l_len_buff = 256;
   // uint32_t l_looper;
    uint8_t writeTest[3];
    uint8_t readTest[3];
    writeTest[0] = 1;	
    writeTest[1] = 2;
    writeTest[2] = 3;

    /*
    for(l_looper=0; l_looper<l_len_buff; ++l_looper)
        l_buff[l_looper] = l_looper;
    */
    file_write_data(g_fd,writeTest,sizeof(writeTest));
    sleep(1);
    memset(l_buff,0,l_len_buff);
    file_read_data(g_fd,writeTest,sizeof(readTest));
    
    /*	
    for(l_looper=0; l_looper<l_len_buff; ++l_looper) {
        printf("%02x",l_buff[l_looper]);
        if(l_buff[l_looper] != l_looper) {
        printf("\r\nSomething went wrong in the loopback data check...%d and %d\r\n",l_buff[l_looper],l_looper) ;
            exit(EXIT_FAILURE);
        }
    }
    */
    //Configuração do display:	
   // int lcd;               
   // wiringPiSetup();        
   // lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
   // lcdPrintf(lcd,"valor %d", writeTest[1]);   	

    printf("valor recebido : %d \n", readTest[0]);
    printf("\r\nThe data loopback was successful!\r\n");
}

static void close_serial_port(void) {
    file_close(g_fd);
}

uint8_t send_to_node(uint8_t *writeTest){
    uint8_t auxTest[8] = writeTest;
    file_write_data(g_fd,auxTest,sizeof(auxTest));
	sleep(1);
    memset(l_buff,0,l_len_buff);
    file_read_data(g_fd,auxTest,sizeof(auxTest));
	printf("\n %d \n", auxTest[0]);
    return auxTest;
}

int main(void) {
    printf("Starting the loopback application...\r\n");

    open_serial_port();

    configure_serial_port();

    //perform_demo();

   
    int aux=0;
    int menu=0; 
    
    uint8_t writeTest[8];
    uint8_t readTest[8];
    char *ptr;
    long value;


    while(aux==0){
        printf("1 -- Solicita a situação atual da nodeMCU\n");
        printf("2 -- Valor de entrada analogica\n");
        printf("3 -- Valor da temperatura\n");
        printf("4 -- Valor da umidade\n");
        printf("5 -- Acender a led\n");
        printf("6 -- Apagar a led\n");
        scanf("%d" , &menu);
        switch(menu){
            case 1:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 3;

                readTest = send_to_node(writeTest);
            
                if(readTest[7] == 0){
                    print_display(0,0);
                }else if (readTest[7] == 9){
                {
                    print_display(9,0);
                }
            break;
            case 2:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 4;

                readTest = send_to_node(writeTest);
                value = strtol(readTest, &ptr, 10);
                print_display(1,value);
            break;
            case 3:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 2;

                readTest = send_to_node(writeTest);
                value = strtol(readTest, &ptr, 10);
                print_display(2,value);
            break;
            case 4:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 1;

                readTest = send_to_node(writeTest);
                value = strtol(readTest, &ptr, 10);
                print_display(3,value);
            break;
            case 5:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 6;

                send_to_node(writeTest);
                print_display(4,value);
            break;
            case 6:
                writeTest[0] = 0;   
                writeTest[1] = 0;
                writeTest[2] = 0;
                writeTest[3] = 0;   
                writeTest[4] = 0;
                writeTest[5] = 0;
                writeTest[6] = 0;   
                writeTest[7] = 5;

                send_to_node(writeTest);
                print_display(5,value);
            default:
                printf("Opção invalida\n\n\n");
                aux=0;
            break;
        }
    }
    close_serial_port();

    return 0;
}
