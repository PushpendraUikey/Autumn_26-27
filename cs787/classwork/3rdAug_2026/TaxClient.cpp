// 23b1023

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class TAXCalculatorProxy {
    sockaddr_in serverAddress{};
    int clientSocket; // Store socket persistently

public:
    TAXCalculatorProxy() : clientSocket(-1) {}

    void setReal(int port, const char* ip) {
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, ip, &serverAddress.sin_addr);

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    }

    int calculate(int income) {
        char buffer[1024];
        sprintf(buffer, "%d", income);
        send(clientSocket, buffer, strlen(buffer), 0);

        int bytes = recv(clientSocket, buffer, sizeof(buffer)-1, 0);

        if(bytes > 0) {
            buffer[bytes] = '\0';
            cout << "Tax to be paid is: " << buffer << endl;
            return atoi(buffer); // Return the tax value as an integer.
        }

        return -1; // Indicate an error
    }

    ~TAXCalculatorProxy() {
        if (clientSocket != -1) {
            close(clientSocket); // Clean up on exit
        }
    }
};

int main()
{
    TAXCalculatorProxy proxy;
    proxy.setReal(8080, "127.0.0.1"); // for local testing.
    int income;
    
    while(true) {
        cout << "Enter income (or -1 to quit): ";
        cin >> income;
        if(income == -1) break;
        proxy.calculate(income);
    }
    
    return 0;
}