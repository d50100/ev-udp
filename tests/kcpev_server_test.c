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

// ����kcpev�ķ����
//

// ����һ��ȫ�ֵ� �Ự 
struct Session{
	IUINT32 conv; // �Ự��id
	IUINT32 data_len; // ���ݳ���
	IUINT32 data_type; // ��������
	int statue; // �Ự��״̬  ��ȡͷ�����Ƕ�ȡ����
	FILE* fp; // �ļ�ָ��
	UT_hash_handle hh; // ��ϣ���
};

typedef struct Session Session;

Session* sessions = NULL;


void recv_cb(KcpevServer *server, Kcpev* client, const char* buf, size_t len)
{
	// ��ȡ�Զ˵�IP��ַ�Ͷ˿ںţ�������
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
		// ��ȡͷ������
		IUINT32 data_len = 0;
		memcpy(&data_len, ptr, 4);
		ptr = ptr + 4;

		debug("recv data_len: %u", data_len);

		IUINT32 data_type = 0;
		memcpy(&data_type, ptr, 4);
		ptr = ptr + 4;
		debug("recv data_type: %u", data_type);

		//// ������д���ļ�
		//FILE* fp = fopen("test.txt", "ab+");
		//if (fp == NULL) {
		//	debug("open file error");
		//	goto error;
		//}

		//// ��ʣ�������д�뵽�ļ���..
		//fwrite(ptr, 1, len - 8, fp);

		//if (data_len == len - 8) {
		//	debug("�ر��ļ�");
		//	fclose(fp);
		//}
		//else {
		//	debug("ʣ��%d�ֽ���Ҫ����", data_len - len);
		//}


		// ����conv ��ȡ�Ự

		// �� buf����ȡ�Ựid


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

