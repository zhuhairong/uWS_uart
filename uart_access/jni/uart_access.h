#ifndef _UART_ACCESS_H__
#define _UART_ACCESS_H__

#include "termios.h"
#include <pthread.h>

typedef void* (*callbackfunsensor)(char* buffer, int size);

class Uart_Access
{
public:
	Uart_Access(const char* dev_name);
	virtual ~Uart_Access();

	int uart_init();
	void uart_exit();
	int uart_send(char* buf, int size);
	int thread_recv();

	// for receive callback
	int set_sensor_cb(callbackfunsensor cb);

protected:
	static void* uart_recv_wrap(void* para);
	int uart_recv();
	
private:
	char m_devName[256];
#define RECV_MAX_SIZE 1024	
	char m_recvBuf[RECV_MAX_SIZE];
	int m_fd;
	int m_exit;
	struct termios oldtio;
	pthread_t m_recvThread;
	fd_set readset; 
	struct timeval tv; 
	callbackfunsensor	m_recv_cb;
};


#endif
