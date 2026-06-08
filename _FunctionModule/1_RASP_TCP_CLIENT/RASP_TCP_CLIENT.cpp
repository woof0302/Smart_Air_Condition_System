#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0)
    {
        std::cout << "Failed to create socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);

    if (inet_pton(AF_INET,


                  "59.10.246.89",
                  &serverAddr.sin_addr) <= 0)
    {
        std::cout << "Invalid IP address." << std::endl;
        close(clientSocket);
        return -1;
    }

    if (connect(clientSocket,
                (sockaddr*)&serverAddr,
                sizeof(serverAddr)) < 0)
    {
        std::cout << "Failed to connect to server." << std::endl;
        close(clientSocket);
        return -1;
    }

    std::cout << "Connected to server successfully." << std::endl;

    int value = 1;

    while (true)
    {
        std::string packet =
            "VS" +
            std::to_string(value) +
            "*";

        ssize_t sendLen = send(
            clientSocket,
            packet.c_str(),
            packet.size(),
            0);

        if (sendLen < 0)
        {
            std::cout << "Failed to send data." << std::endl;
            break;
        }

        std::cout << "[TX] " << packet << std::endl;

        value++;

        std::this_thread::sleep_for(
            std::chrono::seconds(1));
    }

    close(clientSocket);

    return 0;
}
