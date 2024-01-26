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
    // 发送文件 
	FILE* fp = fopen("/root/test.txt", "rb");
	if (fp == NULL) {
		return;
	}
    // 分段发送文件
	char buf[1024];
	while (1) {
		int ret = fread(buf, 1, sizeof(buf), fp);
		if (ret <= 0) {
			break;
		}
		kcpev_send(w->data, buf, ret);
	}
	fclose(fp);
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

	ev_timer* timer = ev_timer_new(loop, send_file, 1, 0);
	ev_timer_start(loop, timer);

    ev_run(loop, 0);
	return 0;
error:
    return -1;
}


