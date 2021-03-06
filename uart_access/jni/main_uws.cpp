#include <uWS.h>
#include <thread>
#include <algorithm>
#include <iostream>

uWS::WebSocket<uWS::SERVER>* g_ws = NULL;
pthread_mutex_t g_mutex;
char sendbuff[256];

void* send_thread(void*)
{
	static int cc = 0;
	while(true)
	{
		sprintf(sendbuff, "abc:%d\n", cc++);
		if(g_ws)
			g_ws->send(sendbuff, strlen(sendbuff), uWS::OpCode::TEXT);
		usleep(30000);
	}
	return 0;
}

int main() {
	pthread_t threadSend;
	pthread_create(&threadSend, NULL, send_thread, NULL);
	pthread_mutex_init(&g_mutex, NULL);

	uWS::Hub h;

    h.onConnection([](uWS::WebSocket<uWS::SERVER> *ws, uWS::HttpRequest req) {
         std::cout << "onConnection"<< std::endl;
		 pthread_mutex_lock(&g_mutex);
		 g_ws = ws;
		 pthread_mutex_unlock(&g_mutex);
    });

    h.onError([](void *user) {
        std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
        exit(-1);
    });

    h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
		//std::cout<<"onMessage:"<<length<<std::endl;
		//ws->send(message, length, opCode);
    });

	h.onDisconnection([](uWS::WebSocket<uWS::SERVER> *ws, int code, char *message, size_t length) {
        std::cout << "CLIENT CLOSE: " << code << std::endl;
		pthread_mutex_lock(&g_mutex);
		g_ws = NULL;
		pthread_mutex_unlock(&g_mutex);
    });

    if (h.listen(12000)) {
        h.run();
    }


	return 0;

//	
//    std::vector<std::thread *> threads(std::thread::hardware_concurrency());
//    std::transform(threads.begin(), threads.end(), threads.begin(), [](std::thread *t) {
//        return new std::thread([]() {
//            uWS::Hub h;
//			h.onConnection([](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
//		        switch ((long) ws->getUserData()) {
//		        case 8:
//		            std::cout << "Client established a remote connection over non-SSL" << std::endl;
//		            ws->close(1000);
//		            break;
//		        case 9:
//		            std::cout << "Client established a remote connection over SSL" << std::endl;
//		            ws->close(1000);
//		            break;
//		        default:
//		            std::cout << "FAILURE: " << ws->getUserData() << " should not connect!" << std::endl;
//		            exit(-1);
//		        }
//		    });
//
//		    h.onError([](void *user) {
//		        std::cout << "FAILURE: Connection failed! Timeout?" << std::endl;
//		        exit(-1);
//		    });
//
//            h.onMessage([](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode) {
//				std::cout<<"onMessage:"<<length<<std::endl;
//				ws->send(message, length, opCode);
//            });
//
//			h.onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
//		        std::cout << "CLIENT CLOSE: " << code << std::endl;
//		    });
//
//            // This makes use of the SO_REUSEPORT of the Linux kernel
//            // Other solutions include listening to one port per thread
//            // with or without some kind of proxy inbetween
//            if (!h.listen(12000, nullptr, uS::ListenOptions::REUSE_PORT)) {
//                std::cout << "Failed to listen" << std::endl;
//            }
//            h.run();
//        });
//    });
//
//    std::for_each(threads.begin(), threads.end(), [](std::thread *t) {
//        t->join();
//    });
}
