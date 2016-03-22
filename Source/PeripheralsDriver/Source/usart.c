#include  <stdio.h>      
#include  <stdlib.h>    
#include  <stdint.h>
#include  <unistd.h>    
#include  <sys/types.h> 
#include  <sys/stat.h>   
#include  <fcntl.h>    
#include  <termios.h>  
#include  <errno.h> 

/*115200 8N1 */
int USART_Config(int fd)
{
	int status;
	struct termios options;

	/*Band Rate 115200*/
	tcgetattr(fd, &options); 
    tcflush(fd, TCIOFLUSH);     
	cfsetispeed(&options, B115200);  
	cfsetospeed(&options, B115200);   

	/*Data Bits 8*/
	options.c_cflag |= CS8;

	/*No Parity*/
	options.c_cflag &= ~PARENB;   
	options.c_iflag &= ~INPCK;

	/*Stop Bit 1*/
	options.c_cflag &= ~CSTOPB;

	/*写入配置*/
	status = tcsetattr(fd, TCSANOW, &options); 

	if (status != 0)
		return -1;
	else
		return 0;
}

