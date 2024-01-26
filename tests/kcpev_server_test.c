#include <kcpev.h>
#include <stdio.h>
#ifdef _WIN32
#   include <winsock2.h>
#   include <WS2tcpip.h>
#   include <stdint.h>
#else
#   include <netdb.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif
#include "dbg.h"

#define BACKLOG 128
#define PORT "33333"

// 基于kcpev的服务端
//

void recv_cb(KcpevServer *server, Kcpev* client, const char* buf, size_t len)
{
	// 获取对端的IP地址和端口号，和数据
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int ret = getpeername(client->udp.sock, (struct sockaddr*)&addr, &addrlen);
	if (ret < 0){
		debug("getpeername error");
	}
	else
	{
		debug("recv from client: %s:%d %s", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buf);
	}
    
    // kcpev_send(client, buf, len);
}

int main()
{
    KcpevServer *kcpev = NULL;
	struct ev_loop *loop = EV_DEFAULT;

    kcpev = kcpev_create_server(loop, PORT, AF_INET, BACKLOG);
    check(kcpev, "init server");

    kcpev_server_set_cb(kcpev, recv_cb, NULL);

    debug("wait for clients...");

    ev_run(loop, 0);
	return 0;
error:
    return -1;

}

