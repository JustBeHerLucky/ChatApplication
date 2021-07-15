#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFF_SIZE 2048

using namespace std;

// Function to recieve response from server
unsigned __stdcall recvThread(void* param) {
	char buff[BUFF_SIZE];
	int ret;
	SOCKET connectedSocket = (SOCKET)param;

	while (true)
	{
		// Receive message
		ret = recv(connectedSocket, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)  continue;
			else cout << "Error " << WSAGetLastError() << endl;
		}
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
			if ((string)buff == "\\out") {
				cout << "Dang xuat thanh cong, Client ngat ket noi trong 5s";
				Sleep(5000);
				exit(0);
			}else cout << buff << endl;
		}
	}


}

int main(int argc, char* argv[]) {

	char host[] = "127.0.0.1";
	int port = 5500;

	// Init winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		cout << "Version is not supported." << endl;
	}

	// Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	cout << "Client started." << endl;

	int timeout = 10000;
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&timeout), sizeof(int));

	// Specify server address
	sockaddr_in	serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, host, &serverAddr.sin_addr);


	// Request to connect server
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr))) {
		cout << "Error! Cannot connect server. " << WSAGetLastError() << endl;
		return 0;
	}

	// Communicate with server
	char buff[BUFF_SIZE];
	int ret, messageLen;

	// Start recieve server response thread
	_beginthreadex(0, 0, recvThread, (void*)client, 0, 0);

	while (true)
	{
		
		// Send message
		gets_s(buff, BUFF_SIZE);
		messageLen = strlen(buff);
		ret = send(client, buff, messageLen, 0);
		if (ret == SOCKET_ERROR) cout << "Error " << WSAGetLastError() << endl;
	}

	cout << "Close client" << endl;

	// Close socket
	closesocket(client);

	// Free winsock
	WSACleanup();

	return 0;
}