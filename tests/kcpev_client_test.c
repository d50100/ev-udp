#include <kcpev.h>
#include <stdio.h>
#ifdef _WIN32
#   include <winsock2.h>
#   include <WS2tcpip.h>
#   include <stdint.h>
#   include <windows.h>
#else
#   include <netdb.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif
#include <ev.h>
#include "kcpev.h"

#include "dbg.h"
#include "test.h"

#define BACKLOG 128
#define PORT "0"

// 基于kcpev的客户端
//

void on_stdin_read(EV_P_ struct ev_watcher *w, int revents, const char *buf, size_t len)
{
	int ret = kcpev_send(w->data, buf, len);
	check(ret >= 0, "");
error:
    return;
}

void recv_cb(void *kcpev, const char *buf, int len)
{
    debug("recv_cb");
}

// 发送文件
void send_file(EV_P_ struct ev_io* w, int revents) {
	debug("send_file"); 

	char file_path[] = "/root/test.txt";

	// 从file_path 中获取文件名
	char* file_name = strrchr(file_path, '/');
	if (file_name == NULL) {
		debug("get file_name error");
		goto error;
	}
	file_name++;

	// 获取文件大小
	FILE* fp = fopen(file_path, "rb");
	if (fp == NULL) {
		debug("open file error");
		goto error;
	}
	fseek(fp, 0, SEEK_END);
	IUINT32 file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fclose(fp);



	char request[1024];
	char *ptr = request;
	IUINT32 file_size_u32 = 12 +file_size;
	memcpy(ptr, &file_size_u32, 4);
	ptr += 4;


	IUINT32 type = 1;
	memcpy(ptr, &type, 4);

	// 发送 8字节的头部
	debug("send 8 bytes: header");
	// 打印字节流
	for (int i = 0; i < 8; i++) {
		printf("%02x ", request[i]);
	}	
	printf("\n");

	kcpev_send(w->data, request, 8);
	
    // 发送文件 
	fp = fopen(file_path, "rb");
	if (fp == NULL) {
		debug("open file error");
		return;
	}



	//// 发送body部分

 //   // 分段发送文件
	//char buf[1024];

	//while (1) {
	//	memset(buf, 0, sizeof(buf));

	//	int ret = fread(buf, 1, sizeof(buf), fp);
	//	if (ret <= 0) {
	//		debug("read file end");
	//		break;
	//	}
	//	debug("send %d bytes: %s", ret, buf);
	//	kcpev_send(w->data, buf, ret);
	//}
	//fclose(fp);
error:
	return;

}

int main()
{
    Kcpev *kcpev = NULL;
	struct ev_loop *loop = EV_DEFAULT;

    kcpev = kcpev_create_client(loop, PORT, AF_INET);
    check(kcpev, "init client");

    int ret = kcpev_connect(kcpev, "10.10.10.8", "33333");
    check(ret >= 0, "connect");

    kcpev_set_cb(kcpev, recv_cb, NULL);

	ev_timer* evh = malloc(sizeof(ev_timer));
	memset(evh, 0, sizeof(ev_timer));
	check_mem(evh);

	evh->data = kcpev;
	ev_timer_init(evh, send_file, 1, 0);
	ev_timer_start(kcpev->loop, evh);


    ev_run(loop, 0);
	return 0;
error:
    return -1;
}


