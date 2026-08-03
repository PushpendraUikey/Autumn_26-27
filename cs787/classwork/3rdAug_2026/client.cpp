// 23b1025 23b1023

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main()
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    inet_pton(AF_INET, "10.51.2.131", &serverAddress.sin_addr);

    connect(clientSocket,
            (sockaddr*)&serverAddress,
            sizeof(serverAddress));

    char buffer[1024];

    strcpy(buffer, "Hello 23b1023");

    send(clientSocket, buffer, strlen(buffer), 0);

    int bytes = recv(clientSocket, buffer, sizeof(buffer)-1, 0);

    if(bytes > 0)
    {
        buffer[bytes] = '\0';
        cout << "Server: " << buffer << endl;
    }

    close(clientSocket);
}