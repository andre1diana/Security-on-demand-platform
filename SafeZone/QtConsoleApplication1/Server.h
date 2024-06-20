#undef UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>

class Server {
private:
	WSADATA wsaData;
	int iResult;
    SOCKET ClientSocket = INVALID_SOCKET;

    static Server* instance;
    Server() {}
    ~Server() {}

public:

    static Server& getInstance();
    static void destroyInstance();

    int startServer();
    int createConnection();
    int receiveData(char* buffer);
    int sendData(const char* buffer, int length);
    void closeServer();
    void sendFile(const char* filename);
    std::string receiveFile();
    bool isClosed();

    void sendInt(int value);

    int receiveInt();

};