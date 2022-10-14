
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT_PATH "/dev/ttyS0" // endereço para a MINI-UART()

//extern void start();
//extern void begin();
//extern void write0();
//extern void write1();
struct termios g_tty;
int g_fd;


// Funções auxiliares
int int_to_binary(int k) {
  if (k == 0)
    return 0;
  if (k == 1)
    return 1;
  return (k % 2) + 10 * int_to_binary(k / 2);
}

// FILE OPERATION
static int file_open_and_get_descriptor(const char *fname) {
  int fd;

  fd = open(fname, O_RDWR | O_NONBLOCK);
  if (fd < 0) {
    printf("Could not open file %s...%d\r\n", fname, fd);
  }
  return fd;
}

static int file_write_data(int fd, uint8_t *buff, uint32_t len_buff) {
  return write(fd, buff, len_buff);
}

static int file_read_data(int fd, uint8_t *buff, uint32_t len_buff) {
  return read(fd, buff, len_buff);
}

static int file_close(int fd) { return close(fd); }

static void open_serial_port(void) {
  g_fd = file_open_and_get_descriptor(SERIAL_PORT_PATH);
  // printf("%d \n", &g_fd);
  if (g_fd < 0) {
    printf("Something went wrong while opening the port...\r\n");
  }
}

static void configure_serial_port(void) {
  if (tcgetattr(g_fd, &g_tty)) {
    printf("Something went wrong while getting port attributes...\r\n");
    exit(EXIT_FAILURE);
  }

  cfsetispeed(&g_tty, B115200);
  cfsetospeed(&g_tty, B115200);

  cfmakeraw(&g_tty);
  // printf("uart configurada!\r\n");
  if (tcsetattr(g_fd, TCSANOW, &g_tty)) {
    printf("Something went wrong while setting port attributes...\r\n");
    exit(EXIT_FAILURE);
  }
}

static void perform_demo(int num) {
  int i = 0;
  uint8_t l_buff[num];
  uint32_t l_len_buff = num;
  uint32_t l_looper;
  for(i=0;i<4;i++){
    l_buff[i] = 0;      
  }
  i=0;
  while (num > 0) {
    uint8_t digit = num % 10;
    l_buff[i] = digit;
   // printf("%0x ", l_buff[i]);
    i++;
    num /= 10;
  }
 /* Loop do código antigo.
   for(l_looper=0; l_looper<l_len_buff; ++l_looper)
   	l_buff[l_looper] = l_looper;
  */	

  file_write_data(g_fd, l_buff, l_len_buff);
  sleep(1);
  memset(l_buff, 0, l_len_buff);
  file_read_data(g_fd, l_buff, l_len_buff);

 // for (l_looper = 0; l_looper < sizeof(l_buff); ++l_looper) {
        // printf("%0x", l_buff[l_looper]);

 // }
  printf("Binario recebido : ");
  for(i=0;i<4;i++){
   printf("%0x ", l_buff[i]);
  }     
  printf("\r\nThe data loopback was successful!\r\n");
}



static void uartLoopDisplay(int num){   //envia e recebe um numero pela UART e então exibe tal numero no display.
  int i = 0;
  
  uint8_t l_buff[4];
  uint32_t l_len_buff = 4;
  uint32_t l_looper;
  for(i=0;i<4;i++){
    l_buff[i] = 0;      
  }
  i=0;
  while (num > 0) {
    uint8_t digit = num % 10;
    l_buff[i] = digit;
   // printf("%0x ", l_buff[i]);
    i++;
    num /= 10;
  }
  l_buff[0] = num;
  /* Loop do código antigo.
   for(l_looper=0; l_looper<l_len_buff; ++l_looper)
   	l_buff[l_looper] = l_looper;
  */
  	
  //Enviando dados para uart.
  file_write_data(g_fd, l_buff, l_len_buff);
  printf("teste");
  sleep(1);
  memset(l_buff, 0, l_len_buff);
  file_read_data(g_fd, l_buff, l_len_buff);

  /* for (l_looper = 0; l_looper < sizeof(l_buff); ++l_looper) {
        // printf("%0x", l_buff[l_looper]);

  }*/
  
  printf("Binario recebido : ");
  for(i=0;i<4;i++){
   printf("%0x ", l_buff[i]);
  }   
  printf("\n");
  //Armazenando dados recebidos nos DB's que vão ser enviados ao display:.
  
 // uint8_t db4 = l_buff[0];   
 // uint8_t db5 = l_buff[1]; 
 // uint8_t db6 = l_buff[2];
 // uint8_t db7 = l_buff[3];
 // begin(0,0,0,1);
 printf("%d", l_buff[0]);
 if(l_buff[0]==0){
  extern void start();
  extern void write0();
	start();
	 write0();
 }
 else{
  extern void start();
  extern void write1();
   start();
   write1();	
 }
  
  
  printf("\r\nThe data loopback was successful!\r\n");	

}


static void close_serial_port(void) { file_close(g_fd); }




int main(void) {
  // printf("Starting the loopback application...\r\n");
  int num, res;
  
  printf("Informe o numero a ser enviado: \n");
  scanf("%d", &num);
  res = int_to_binary(num);

  open_serial_port();

  configure_serial_port();
 printf("01");
 // start();
 // perform_demo(res);
  uartLoopDisplay(num);

  close_serial_port();

  return 0;
}




