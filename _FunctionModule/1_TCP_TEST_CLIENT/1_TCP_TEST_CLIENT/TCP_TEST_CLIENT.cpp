#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup 실패\n";
        return -1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "소켓 생성 실패\n";
        WSACleanup();
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);

    InetPtonA(
        AF_INET,
        "127.0.0.1",
        &serverAddr.sin_addr);

    if (connect(
        clientSocket,
        (sockaddr*)&serverAddr,
        sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "서버 연결 실패\n";

        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "서버 연결 성공\n";

    int value = 1;

    while (true)
    {
        std::string packet =
            "VS" +
            std::to_string(value) +
            "*";

        int sendLen = send(
            clientSocket,
            packet.c_str(),
            static_cast<int>(packet.size()),
            0);

        if (sendLen == SOCKET_ERROR)
        {
            std::cout << "전송 실패\n";
            break;
        }

        std::cout << "[송신] " << packet << std::endl;

        value++;

        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}