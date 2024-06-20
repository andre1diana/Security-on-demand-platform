#include "Server.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <cstdint>

#define BUFF_LEN 512

Server* Server::instance = nullptr;

int Server::startServer()
{
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        Logger::getInstance().logError(1, "Server (startServer) : Failed to start up the winsock API!");
        return -1;
    }
}

int Server::createConnection()
{
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in coming_addr;

    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    // 0.0.0.0 => any IP 
    inet_pton(AF_INET, "0.0.0.0", &saServer.sin_addr);
    saServer.sin_port = htons(12345);

    bind(listen_sock, (sockaddr*)&saServer, sizeof(sockaddr_in));
    listen(listen_sock, SOMAXCONN);

    int size = sizeof(coming_addr);
    SOCKET client_sock = accept(listen_sock, (sockaddr*)&coming_addr, &size);
    if (client_sock == INVALID_SOCKET)
    {
        Logger::getInstance().logError(WSAGetLastError(), "Server(startServer) : connect function failed.");
        return -1;
    }
    else {
        char buffer[36];
        inet_ntop(AF_INET, &coming_addr.sin_addr.s_addr, buffer, 36);
        Logger::getInstance().logMessage(1, "Connected!", buffer);
        ClientSocket = client_sock;
    }
}

Server& Server::getInstance()
{
    if (!Server::instance)
    {
        Server::instance = new Server();
    }
    return (*Server::instance);
}

int Server::receiveData(char* buffer) {
    int totalBytesReceived = 0;
    int dataSize = receiveInt();
    while (totalBytesReceived < dataSize) {
        int bytesReceived = recv(ClientSocket, buffer + totalBytesReceived, dataSize - totalBytesReceived, 0);
        if (bytesReceived <= 0) {
            Logger::getInstance().logError(1, "Server (receiveData) : failed to receive data");
            break;
        }
        totalBytesReceived += bytesReceived;
    }
    Logger::getInstance().logMessage(1, "Received data from client!", "");
    return dataSize;
}

void Server::destroyInstance()
{
    if (Server::instance)
    {
        delete Server::instance;
        Server::instance = nullptr;
    }
}


int Server::sendData(const char* buffer, int length)
{
    sendInt(length);
    int iSendResult = send(ClientSocket, buffer, length, 0);
    Logger::getInstance().logMessage(1, "Send data to client!", "");
    if (iSendResult == SOCKET_ERROR) {
        Logger::getInstance().logError(WSAGetLastError(), "Server (sendData) : send failed.");
        closesocket(ClientSocket);
        closeServer();
        return 1;
    }
    return 0;
}

void Server::closeServer()
{
    closesocket(ClientSocket);
    WSACleanup();
}

void Server::sendFile(const char* filename)
{
    Logger::getInstance().logMessage(1, "Server start to sent file to client", "");
    int outFileLen = strlen(filename);
    Server::getInstance().sendData(filename, outFileLen);

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        Logger::getInstance().logError(1, "Server (sendFile) : Could not open file.");
        return;
    }

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    sendInt(fileSize);

    char buffer[BUFF_LEN];
    std::streamsize rc;
    int totalBytesSent = 0;
    int wc;
    while (file.read(buffer, sizeof(buffer))) {
        rc = file.gcount();
        //sendData(buffer, rc);
        wc = send(ClientSocket, buffer, rc, 0);
        Logger::getInstance().logMessageAndInt(1, "Sent total of bytes : ", wc);
        totalBytesSent += rc;
    }

    if (file.eof()) {
        rc = file.gcount();
        //sendData(buffer, static_cast<int>(rc));
        wc = send(ClientSocket, buffer, rc, 0);
        Logger::getInstance().logMessageAndInt(1, "Sent total of bytes : ", wc);
        totalBytesSent += rc;
    }

    file.close();
    if (totalBytesSent == fileSize) {
        Logger::getInstance().logMessage(1, "File sent successfully.", filename);
    }
    else
    {
        Logger::getInstance().logError(1, "Server (receiveFile) : File not received totally!");
    }
}

std::string Server::receiveFile()
{
    Logger::getInstance().logMessage(1, "Server receive file : ", "");
    char buff[256];
    int size = Server::getInstance().receiveData(buff);
    buff[size] = '\0';
    std::string filename(buff);

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        Logger::getInstance().logError(1, "Server (receiveFile) : Could not open file to write.");
        return "";
    }

    int fileSize = receiveInt();
    int totalBytesReceived = 0;
    char buffer[BUFF_LEN];

    while (totalBytesReceived < fileSize) {
        int bytesReceived = recv(ClientSocket, buffer, BUFF_LEN, 0);
        if (bytesReceived <= 0) {
            Logger::getInstance().logError(WSAGetLastError(), "Server (receiveFile) : recv failed.");
            break;
        }
        totalBytesReceived += bytesReceived;
        file.write(buffer, bytesReceived);
    }

    file.close();
    if (totalBytesReceived == fileSize) {
        Logger::getInstance().logMessage(1, "File received and saved successfully.", filename);
    }
    else
    {
        Logger::getInstance().logError(1, "Server (receiveFile) : File not received totally!");
    }
    return filename;
}

bool Server::isClosed()
{
    if (ClientSocket == NULL)
    {
        return true;
    }
    return false;
}

void Server::sendInt(int value)
{
    int result = send(ClientSocket, reinterpret_cast<const char*>(&value), sizeof(value), 0);
    if (result == SOCKET_ERROR) {
        Logger::getInstance().logError(WSAGetLastError(), "Error Server (sendInt) : Send int failed.");
    }
    Logger::getInstance().logMessageAndInt(1, "Send int : ", value);
}

int Server::receiveInt()
{
    int value;
    int result = recv(ClientSocket, reinterpret_cast<char*>(&value), sizeof(value), 0);
    if (result == SOCKET_ERROR) {
        Logger::getInstance().logError(WSAGetLastError(), "Error Server (receiveInt) : Receive int failed.");
        return -1;
    }
    Logger::getInstance().logMessageAndInt(1, "Received int : ", value);
    return value;
}