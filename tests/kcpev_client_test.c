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
    // 发送文件 
	FILE* fp = fopen("/root/test.txt", "rb");
	if (fp == NULL) {
		debug("open file error");
		return;
	}
    // 分段发送文件
	char buf[1024];
	

	while (1) {
		memset(buf, 0, sizeof(buf));

		int ret = fread(buf, 1, sizeof(buf), fp);
		if (ret <= 0) {
			debug("read file end");
			break;
		}
		debug("send %d bytes: %s", ret, buf);
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


