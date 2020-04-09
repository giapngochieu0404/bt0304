

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);

SOCKET clients[64];
int numClients;
char id[64];
char client_id[256];


bool check(char buf[256]) {
	if (strlen(buf) > 11) {
		strcpy(id, buf + 11);
		id[strlen(id)] = 0;

		
		for (int i = 0; i < strlen(id); i++) {
			if (id[i] != ' ')
				return true;
		}

		
		strncpy(client_id, buf, 11);
		client_id[11] = 0;
		if (strcmp(client_id, "client_id: ") == 0)
			return true;
	}
	
	return false;
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	numClients = 0;

	
	while (1)
	{
		printf("Waiting for new client\n");
		SOCKET client = accept(listener, NULL, NULL);
		printf("New client accepted: %d\n", client);

		clients[numClients] = client;
		numClients++;

		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;

	int ret;
	char buf[256];
	char input[256];

	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

	
		buf[ret] = 0; 
		
		
		if (check(buf)==true) 
			break;
		
		
		char a[256] = "nhap lai id:";
		send(client, a, strlen(a), 0);
	}

	while (1)
	{
		char result[256];
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

	
		buf[ret] = 0;

		int i, n = strlen(id);
		
		strncpy(result, id, n - 1);
		result[n-1] = ':';

		
		for (i = 0; i < strlen(buf); i++) {
			result[n+i] = buf[i];
		}
		
		result[n + strlen(buf)] = 0;
		printf("Received: %s\n", buf);
		
			
			for (int i = 0; i < numClients; i++)
				if (clients[i] != client)
					send(clients[i], result, strlen(result), 0);
	}

	
	RemoveClient(client);
	closesocket(client);
}

void RemoveClient(SOCKET client)
{
	int i = 0;
	for (; i < numClients; i++)
		if (clients[i] == client) break;

	if (i < numClients - 1)
		clients[i] = clients[numClients - 1];

	numClients--;
}

