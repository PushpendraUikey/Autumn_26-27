// 23b1023 - 23b1025

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main()
{
    // creating socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying the address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // binding socket.
    bind(serverSocket, (struct sockaddr*)&serverAddress,
         sizeof(serverAddress));

    // listening to the assigned socket
    listen(serverSocket, 5);

    // accepting connection request
    int clientSocket
        = accept(serverSocket, nullptr, nullptr);
    // recieving data
    char buffer[1024];

while(true)
{
    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(clientSocket, buffer, sizeof(buffer)-1, 0);

    if(bytes <= 0)
        break;

    buffer[bytes] = '\0';

    cout << "Client: " << buffer << endl;

    if(buffer[0] == 'q')
        break;

    strcpy(buffer, "Got your message!");

    send(clientSocket, buffer, strlen(buffer), 0);
}

close(clientSocket);
close(serverSocket);
    return 0;
}