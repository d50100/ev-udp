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

// 定义一个全局的 会话 
struct Session{
	IUINT32 conv; // 会话的id
	IUINT32 data_len; // 数据长度
	IUINT32 data_type; // 数据类型
	int statue; // 会话的状态  读取头部还是读取数据
	FILE* fp; // 文件指针
	UT_hash_handle hh; // 哈希句柄
};

typedef struct Session Session;

Session* sessions = NULL;


void recv_cb(KcpevServer *server, Kcpev* client, const char* buf, size_t len)
{
	// 获取对端的IP地址和端口号，和数据
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int ret = getpeername(client->udp.sock, (struct sockaddr*)&addr, &addrlen);
	if (ret < 0) {
		debug("getpeername error");
	}
	else
	{
		debug("recv from client: %s:%d %s", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buf);
	}

	char *ptr = buf;
	if (len >= 8) {
		// 提取头部数据
		IUINT32 data_len = 0;
		memcpy(&data_len, ptr, 4);
		ptr = ptr + 4;

		debug("recv data_len: %u", data_len);

		IUINT32 data_type = 0;
		memcpy(&data_type, ptr, 4);
		ptr = ptr + 4;
		debug("recv data_type: %u", data_type);

		//// 将数据写入文件
		//FILE* fp = fopen("test.txt", "ab+");
		//if (fp == NULL) {
		//	debug("open file error");
		//	goto error;
		//}

		//// 将剩余的数据写入到文件中..
		//fwrite(ptr, 1, len - 8, fp);

		//if (data_len == len - 8) {
		//	debug("关闭文件");
		//	fclose(fp);
		//}
		//else {
		//	debug("剩余%d字节需要接收", data_len - len);
		//}


		// 根据conv 获取会话

		// 从 buf中提取会话id


	}

error:
	debug("recv_cb");



    
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

