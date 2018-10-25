#include <stdio.h>  
#include <sys/time.h>  
#include <string.h>   
#include<sys/stat.h>  
#include<fcntl.h>  
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "uart_access.h"
#include "globle.h"
#include <errno.h>
#include <unistd.h>


#define BAUDRATE B115200 //B9600 //B115200
/*
int tcgetattr(int fd, struct  termios *s)
{
	return ioctl(fd, TCGETS, s);
}

int tcsetattr(int fd, int __opt, const struct termios *s)
{
	return ioctl(fd, __opt, (void*)s);
}

int tcflush(int fd, int  __queue)
{
	return ioctl(fd, TCFLSH, (void*)(int)__queue);
}
*/
Uart_Access::Uart_Access(const char* dev_name)
{
	strcpy(m_devName, dev_name);
	m_fd = -1;
	m_exit = 0;
	m_recv_cb = NULL;
}

Uart_Access::~Uart_Access()
{
	if(m_fd >= 0)
		uart_exit();
}

int SetNcMode(int fd)
{
    struct termios options;
    // get current port settings
    if (tcgetattr(fd, &options) != 0)
    {
        IPRINTF("Setup Serial 1");
		return false;
    } 

    bzero(&options, sizeof(options)); 
    options.c_cflag |= BAUDRATE | CS8 | CLOCAL | CREAD; 
    options.c_iflag = IGNPAR | ICRNL; 
    options.c_oflag = 0;           
				            
    options.c_lflag |= !ICANON;           
    options.c_cc[VINTR]    = 0;
    options.c_cc[VQUIT]    = 0;
    options.c_cc[VERASE]   = 0;
    options.c_cc[VKILL]    = 0;
    options.c_cc[VEOF]     = 4;
    options.c_cc[VTIME]    = 0;
    options.c_cc[VMIN]     = 4;
    options.c_cc[VSWTC]    = 0;
    options.c_cc[VSTART]   = 0;
    options.c_cc[VSTOP]    = 0;
    options.c_cc[VSUSP]    = 0;
    options.c_cc[VEOL]     = 0;
    options.c_cc[VREPRINT] = 0;
    options.c_cc[VDISCARD] = 0;
    options.c_cc[VWERASE]  = 0;
    options.c_cc[VLNEXT]   = 0;
    options.c_cc[VEOL2]    = 0;
						         
    tcflush(fd, TCIFLUSH); 
    tcsetattr(fd,TCSANOW,&options);
    
    return true;
}

int Uart_Access::uart_init()
{
	IPRINTF("Iam m_devName:%s\n", m_devName);
	m_exit = 0;
	struct termios newtio;  
	
	m_fd=open(m_devName, O_RDWR | O_NOCTTY | O_NONBLOCK/*| O_NDELAY*/);  
    if(m_fd<0)  
    {  
        EPRINTF("open error :%s, %s\n", m_devName, strerror(errno));  
        return -1;
    } 

	/*将目前终端机的结构保存至oldtio结构*/  
    tcgetattr(m_fd, &oldtio);  
	
    /*清除newtio结构，重新设置通信协议*/  
    //bzero(&newtio, sizeof(newtio));  
	
    /*通信协议设为8N1,8位数据位,N没有效验,1位结束位*/  
    //newtio.c_cflag = BAUDRATE |CS8|CLOCAL|CREAD;  
    //newtio.c_iflag = IGNPAR;  
    //newtio.c_oflag = 0;  
	
    /*设置为正规模式*/  
    //newtio.c_lflag = ICANON;  
	
    /*清除所有队列在串口的输入*/  
    //tcflush(m_fd, TCIFLUSH);   
	
	/*新的termios的结构作为通信端口的参数*/  
    //tcsetattr(m_fd, TCSANOW, &newtio); 


	SetNcMode(m_fd);

	pthread_create(&m_recvThread, NULL, uart_recv_wrap, this);

	IPRINTF("\n");

	return 0;
}

void Uart_Access::uart_exit()
{
	m_exit = 1;
	//m_recvThread->stop_thread();
	//m_recvThread.clear();
	//m_recvThread = NULL;
	pthread_join(m_recvThread, NULL);
	
	close(m_fd); 
	m_fd = -1;
	
	m_recv_cb = NULL;
	
    /*还原旧的通信端口参数*/  
    tcsetattr(m_fd, TCSANOW, &oldtio);  
}

int Uart_Access::uart_send(char* buf, int size)
{
	int sendsize = 0;
	if(buf == NULL)
	{
		EPRINTF("uart_send buf == NULL\n");
		return -1;
	}
	if(size < 0)
	{
		EPRINTF("size:%d < 0\n", size);
		return -1;
	}

	sendsize = write(m_fd, buf, size);
	DPRINTF("send buf:%s, send size:%d\n", buf, size);
	return sendsize;
}

int Uart_Access::thread_recv()
{
	int MaxFd = m_fd; 
	FD_ZERO(&readset);
	FD_SET(m_fd, &readset);
	tv.tv_sec = 0;
	tv.tv_usec = 500000;

	int ret = select(MaxFd+1, &readset, NULL, NULL, &tv);
	if(ret < 0)
	{
		EPRINTF("select failed, ret:%d\n", ret);
	}

	if(FD_ISSET(m_fd, &readset))
	{
		memset(m_recvBuf, 0, RECV_MAX_SIZE);
		int size = read(m_fd, m_recvBuf, RECV_MAX_SIZE);
		IPRINTF("size:%d\n", size);
		if(m_recv_cb && size > 0)
			m_recv_cb(m_recvBuf, size);

		m_recvBuf[size] = '\0';
		IPRINTF("recv########################################:%s\n", m_recvBuf);
	}
	return 0;
}

int Uart_Access::set_sensor_cb(callbackfunsensor cb)
{
	if(cb == NULL)
	{
		EPRINTF("cb null\n");
		return -1;
	}
	m_recv_cb = cb;
	return 0;
}

void* Uart_Access::uart_recv_wrap(void* para)
{
	return (void*)static_cast<Uart_Access*>(para)->uart_recv();
}

int Uart_Access::uart_recv()
{
	while(!m_exit)
	{
		int MaxFd = m_fd; 
		FD_ZERO(&readset);
		FD_SET(m_fd, &readset);
		tv.tv_sec = 0;
		tv.tv_usec = 500000;

		int ret = select(MaxFd+1, &readset, NULL, NULL, &tv);
		if(ret < 0)
		{
			EPRINTF("select failed, ret:%d\n", ret);
		}

		if(FD_ISSET(m_fd, &readset))
		{
			memset(m_recvBuf, 0, RECV_MAX_SIZE);
			int size = read(m_fd, m_recvBuf, RECV_MAX_SIZE);
			IPRINTF("size:%d\n", size);
			if(m_recv_cb && size > 0)
				m_recv_cb(m_recvBuf, size);

			m_recvBuf[size] = '\0';
			IPRINTF("recv########################################:%s\n", m_recvBuf);
		}
	}
	return 0;
}


