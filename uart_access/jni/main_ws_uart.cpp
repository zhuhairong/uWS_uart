#include <stdio.h>

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


#include "uart_access.h"
#include "globle.h"
extern "C"
{
#include "websock.h"
#include "api.h"
}


Uart_Access* ua = NULL;
libwebsock_client_state *g_state = NULL;
Uart_Access* g_ua = NULL;

void* recv_callback(char* buff, int size)
{
	printf("callback\n");
	if(g_state)
		libwebsock_send_binary(g_state, buff, size);
	return 0;
}

int onopen(libwebsock_client_state *state)
{
	fprintf(stderr, "onopen: %d\n", state->sockfd);
	return 0;
}

int onclose(libwebsock_client_state *state)
{
	fprintf(stderr, "onclose: %d\n", state->sockfd);
	return 0;
}

int onmessage(libwebsock_client_state *state, libwebsock_message *msg)
{
	fprintf(stderr, "only libevent Received message from client: %d\n", state->sockfd);
	fprintf(stderr, "Message opcode: %d\n", msg->opcode);
	fprintf(stderr, "Payload Length: %llu\n", msg->payload_len);
	fprintf(stderr, "Payload: %s\n", msg->payload);
	//now let's send it back.
	//libwebsock_send_text(state, msg->payload);
	
	g_state = state;
	
	if(g_ua)
		g_ua->uart_send(msg->payload, msg->payload_len);

	return 0;
}

int main(int argc, char** argv)
{
	int g_exit = 0;
	ua = new Uart_Access("/dev/ttyS2");///dev/ttyS4");
	int ret = ua->uart_init();
	if(ret < 0)
	{
		printf("init uart fail\n");
		return -1;
	}
	
	g_ua = ua;

	ua->set_sensor_cb(recv_callback);
	
	
	printf("zhr only libevent\n");
	libwebsock_context *ctx = NULL;

  	ctx = libwebsock_init();
	printf("ctx: %p \n", ctx);
	if(ctx == NULL)
	{
		printf("libwebsock_init Failed. \n");
		return 0;
	}

	//dl_icuuc_init();

	char *port = "12000";

	libwebsock_bind(ctx, "192.168.2.139", port);

	ctx->onmessage = onmessage;
 	ctx->onopen = onopen;
	ctx->onclose = onclose;

	printf("libwebsock_wait start \n");
	libwebsock_wait(ctx);
	printf("libwebsock_wait end \n");
	
	/*
	while(!g_exit)
	{
		usleep(1000*100);
	}*/

	delete ua;
	
	return 0;
}

