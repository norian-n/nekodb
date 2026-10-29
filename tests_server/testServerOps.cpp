/* #include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h> */
#include "testTemplate.h"
#include "../server/egServer.h"

int main() {

    EgServer theServer;
    std::string reqStr;

    theServer.openSocket();
    theServer.rcvRequest(reqStr);
    std::cout << "reqStr: " << reqStr << std::endl;

    theServer.closeSocket();

    contributeTestResultToStatistics( testInDevelopment ); // testLayers.layersStorage.nodesContainer-> nodesCount == 3 )
}
/*    
    // 1. Create a socket (IPv4, TCP)
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket. Error: " << strerror(errno) << std::endl;
        return 1;
    }

    // Hint: Allow immediate reuse of the port after stopping the server
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Bind the socket to an IP address and Port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;          // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Bind to any available network interface
    serverAddress.sin_port = htons(8080);        // Port 8080 (converted to Network Byte Order)

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed. Error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    // 3. Listen for incoming connections (Queue up to 5 pending connections)
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Listen failed. Error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    // 4. Accept a client connection
    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLength);
    
    if (clientSocket < 0) {
        std::cerr << "Accept failed. Error: " << strerror(errno) << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected successfully!" << std::endl;

    // 5. Read data from the client and echo it back
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived > 0) {
        std::cout << "Received from client: " << buffer << std::endl;
        
        // Echo back the exact same message
        send(clientSocket, buffer, bytesReceived, 0);
    } else if (bytesReceived == 0) {
        std::cout << "Client disconnected." << std::endl;
    } else {
        std::cerr << "Receive failed." << std::endl;
    }

    // 6. Clean up and close sockets
    close(clientSocket);
    close(serverSocket);
    std::cout << "Server closed." << std::endl;

    return 0;
} */