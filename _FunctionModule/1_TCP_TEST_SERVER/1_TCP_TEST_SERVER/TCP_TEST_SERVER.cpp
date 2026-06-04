#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup 실패\n";
        return -1;
    }

    SOCKET serverSocket =
        socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "소켓 생성 실패\n";
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(
        serverSocket,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "bind 실패\n";

        closesocket(serverSocket);
        WSACleanup();

        return -1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "listen 실패\n";

        closesocket(serverSocket);
        WSACleanup();

        return -1;
    }

    std::cout << "클라이언트 접속 대기중...\n";

    sockaddr_in clientAddr{};
    int clientSize = sizeof(clientAddr);

    SOCKET clientSocket =
        accept(
            serverSocket,
            (sockaddr*)&clientAddr,
            &clientSize);

    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "accept 실패\n";

        closesocket(serverSocket);
        WSACleanup();

        return -1;
    }

    std::cout << "클라이언트 연결 성공\n";

    char recvBuffer[1024];

    std::string packetBuffer;

    while (true)
    {
        int recvLen =
            recv(
                clientSocket,
                recvBuffer,
                sizeof(recvBuffer),
                0);

        if (recvLen <= 0)
        {
            std::cout << "클라이언트 연결 종료\n";
            break;
        }

        packetBuffer.append(
            recvBuffer,
            recvLen);

        size_t pos;

        while ((pos = packetBuffer.find('*'))
            != std::string::npos)
        {
            std::string packet =
                packetBuffer.substr(
                    0,
                    pos + 1);

            packetBuffer.erase(
                0,
                pos + 1);

            std::cout
                << "[수신 패킷] "
                << packet
                << std::endl;
        }
    }

    closesocket(clientSocket);
    closesocket(serverSocket);

    WSACleanup();

    return 0;
}