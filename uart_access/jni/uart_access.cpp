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

/******************************************************************* 
* ���ƣ�                UART0_Set 
* ���ܣ�                ���ô�������λ��ֹͣλ��Ч��λ 
* ��ڲ�����        fd        �����ļ������� 
*                              speed     �����ٶ� 
*                              flow_ctrl   ���������� 
*                           databits   ����λ   ȡֵΪ 7 ����8 
*                           stopbits   ֹͣλ   ȡֵΪ 1 ����2 
*                           parity     Ч������ ȡֵΪN,E,O,,S 
*���ڲ�����          ��ȷ����Ϊ1�����󷵻�Ϊ0 
*******************************************************************/  
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)  
{  
     
	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};  
           
	struct termios options;  
     
	/*tcgetattr(fd,&options)�õ���fdָ��������ز������������Ǳ�����options,�ú��������Բ��������Ƿ���ȷ���ô����Ƿ���õȡ������óɹ�����������ֵΪ0��������ʧ�ܣ���������ֵΪ1. 
    */  
	if( tcgetattr( fd,&options)  !=  0)  
	{  
		perror("SetupSerial 1");      
		return(-1);   
	}  
    
    //���ô������벨���ʺ����������  
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       
     
    //�޸Ŀ���ģʽ����֤���򲻻�ռ�ô���  
    options.c_cflag |= CLOCAL;  
    //�޸Ŀ���ģʽ��ʹ���ܹ��Ӵ����ж�ȡ��������  
    options.c_cflag |= CREAD;  
    
    //��������������  
    switch(flow_ctrl)  
    {  
        
		case 0 ://��ʹ��������  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
		case 1 ://ʹ��Ӳ��������  
              options.c_cflag |= CRTSCTS;  
              break;  
		case 2 ://ʹ�����������  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //��������λ  
    //����������־λ  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
		case 5    :  
                     options.c_cflag |= CS5;  
                     break;  
		case 6    :  
                     options.c_cflag |= CS6;  
                     break;  
		case 7    :      
                 options.c_cflag |= CS7;  
                 break;  
		case 8:      
                 options.c_cflag |= CS8;  
                 break;    
		default:     
                 fprintf(stderr,"Unsupported data size\n");  
                 return (-1);   
    }  
    //����У��λ  
    switch (parity)  
    {    
		case 'n':  
		case 'N': //����żУ��λ��  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
		case 'o':    
		case 'O'://����Ϊ��У��      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
		case 'e':   
		case 'E'://����ΪżУ��    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
		case 's':  
		case 'S': //����Ϊ�ո�   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    
                 fprintf(stderr,"Unsupported parity\n");      
                 return (-1);   
    }   
    // ����ֹͣλ   
    switch (stopbits)  
    {    
		case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
		case 2:     
                 options.c_cflag |= CSTOPB; break;  
		default:     
                       fprintf(stderr,"Unsupported stop bits\n");   
                       return (-1);  
    }  
     
	//�޸����ģʽ��ԭʼ�������  
	options.c_oflag &= ~OPOST;  
    
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
	//options.c_lflag &= ~(ISIG | ICANON);  
     
    //���õȴ�ʱ�����С�����ַ�  
    options.c_cc[VTIME] = 1; /* ��ȡһ���ַ��ȴ�1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* ��ȡ�ַ������ٸ���Ϊ1 */  
     
    //�����������������������ݣ����ǲ��ٶ�ȡ ˢ���յ������ݵ��ǲ���  
    tcflush(fd,TCIFLUSH);  
     
    //�������� (���޸ĺ��termios�������õ������У�  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
	{  
		perror("com set error!\n");    
		return (-1);   
	}  
    return (0);   
}

int Uart_Access::uart_init(int speed)
{
	IPRINTF("Iam m_devName:%s\n", m_devName);
	m_exit = 0;
	struct termios newtio;  
	
	m_fd = open(m_devName, O_RDWR | O_NOCTTY | O_NONBLOCK/*| O_NDELAY*/);  
    if(m_fd<0)  
    {  
        EPRINTF("open error :%s, %s\n", m_devName, strerror(errno));  
        return -1;
    }
	
	if(UART_Set(m_fd, speed, 0, 8, 1, 'N') < 0)
	{
		EPRINTF("set uart error\n");
		return -1;
	}

	pthread_create(&m_recvThread, NULL, uart_recv_wrap, this);

	IPRINTF("\n");

	return 0;
}

void Uart_Access::uart_exit()
{
	m_exit = 1;
	pthread_join(m_recvThread, NULL);
	
	close(m_fd); 
	m_fd = -1;
	
	m_recv_cb = NULL;
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

			printf("recv########################################\n");
			for(int i = 0; i < size; i++)
			{
				printf("[%d]:0x%x ", i, m_recvBuf[i]);
			}
			printf("\nrecv########################################end\n");
		}
	}
	return 0;
}


