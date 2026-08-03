// 23b1023

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

class TAXCalculator {
public:
    virtual int calculate(int) = 0;
};

class TAXCalculatorReal : public TAXCalculator {
public:
    int calculate(int income) override {
        if(income < 0) {
            cout << "Income cannot be negative." << endl;
            return -1; // Indicate an error
        } else if(income < 1200000) {
            return 0; 
        } else if(income < 3000000) {
            return income * 0.15; 
        } else {
            return income * 0.2; 
        }
    }
};

int main()
{
    TAXCalculatorReal *taxCalculator = new TAXCalculatorReal();

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    
    char buffer[1024];

    // Loop over the same connection
    while(true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(clientSocket, buffer, sizeof(buffer)-1, 0);

        if(bytes <= 0) break;
        if(buffer[0] == 'q') break;
            
        int income = atoi(buffer);
        int tax = taxCalculator->calculate(income);

        cout << "Client requested income: " << income << " | Calculated Tax: " << tax << endl;

        // Sending the tax back to the client
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", tax);
        send(clientSocket, buffer, strlen(buffer), 0);
    }

    close(clientSocket);
    close(serverSocket);
    delete taxCalculator;
    
    return 0;
}