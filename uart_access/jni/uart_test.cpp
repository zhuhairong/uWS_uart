#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>


#include "uart_access.h"
#include "globle.h"


Uart_Access* ua = NULL;

void* recv_callback(char* buff, int size)
{
	printf("callback1\n");
	return 0;
}

int main(int argc, char** argv)
{
	int g_exit = 0;
	ua = new Uart_Access("/dev/ttyS1");///dev/ttyS4");
	int ret = ua->uart_init(9600);
	if(ret < 0)
	{
		printf("init uart fail\n");
		return -1;
	}

	ua->set_sensor_cb(recv_callback);
	
	while(!g_exit)
	{
		char buff[10];
		for(int i = 0;i <8; i++)
			buff[i] = i;

		ua->uart_send(buff, 8);
		usleep(1000*1000);
	}

	delete ua;
	
	return 0;
}


