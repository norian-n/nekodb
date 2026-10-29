#include "egServer.h"

bool EgServer::openSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Create a socket (IPv4, TCP)
    if (serverSocket < 0) {
        EG_LOG_STUB << "Failed to create socket. Error: " << strerror(errno) << FN;
        return 1;
    }
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // Allow immediate reuse of the port after stopping the server

    sockaddr_in serverAddress{}; // Bind the socket to an IP address and Port
    serverAddress.sin_family = AF_INET;          // IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Bind to any available network interface
    serverAddress.sin_port = htons(serverPort);  // Port 8080 (converted to Network Byte Order)
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        EG_LOG_STUB << "Bind failed. Error: " << strerror(errno) << FN;
        close(serverSocket);
        return 1;
    }
    if (listen(serverSocket, 5) < 0) { // listen for incoming connections (Queue up to 5 pending connections)
        EG_LOG_STUB << "Listen failed. Error: " << strerror(errno) << FN;
        close(serverSocket);
        return 1;
    }
    EG_LOG_STUB << "Server is listening on port " << serverPort << FN;
    return true; 
}

void EgServer::closeSocket() {
    close(serverSocket);
    EG_LOG_STUB << "Server socket closed" << FN;
 }

bool EgServer::rcvRequest(std::string& theRequest) {
    sockaddr_in clientAddress{}; // Accept a client connection
    socklen_t clientLength = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLength);
    if (clientSocket < 0) {
        EG_LOG_STUB << "Accept failed. Error: " << strerror(errno) << FN;
        close(serverSocket);
        return false;
    }
    EG_LOG_STUB << "Client connected successfully!" << FN;
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    std::string tmpStr (buffer, bytesReceived);
    theRequest = tmpStr;
    if (bytesReceived > 0) {
        EG_LOG_STUB << "Received from client: " << tmpStr << " bytes : " << (int) bytesReceived << FN;
        send(clientSocket, buffer, bytesReceived, 0); // Echo back the exact same message
        return true;
    } else if (bytesReceived == 0) {
        EG_LOG_STUB << "Client disconnected." << FN;
    } else {
        EG_LOG_STUB << "Receive failed." << FN;
    }
    return false; 
}

bool EgServer::rcvByteArray(EgByteArrayAbstractType& byteArray) {
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    byteArray.dataSize = bytesReceived;
    // theRequest = tmpStr;
    if (bytesReceived > 0) {
        byteArray.reallocDataChunk(bytesReceived);
        memcpy((void*)byteArray.dataChunk, (void*) buffer, byteArray.dataSize);
        PrintByteArray(byteArray);
        // EG_LOG_STUB << "Received from client: " << tmpStr << " bytes : " << (int) bytesReceived << FN;
        // send(clientSocket, buffer, bytesReceived, 0); // Echo back the exact same message
        return true;
    } else if (bytesReceived == 0) {
        EG_LOG_STUB << "Client disconnected." << FN;
    } else {
        EG_LOG_STUB << "Receive failed." << FN;
    }
    return false;     
}


bool EgServer::rcvDataNode(EgDataNode& theNode) {


    return true; 
}

bool EgServer::sendDataNode(EgDataNode& theNode) {


    return true; 
}