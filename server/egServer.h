#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../nodes/egDataNode.h"
#include "egLiteralsServer.h"

class EgServer { // "type" means c++ type, data metatype called "blueprint"
public:
    int serverSocket { 0 };
    int clientSocket { 0 };
    int serverPort   { defaultServerPort };
    int opt          { 1 }; // setsockopt aux data
    char buffer[defaultBufSize]; // FIXME stub to read data from the client

    EgServer() {}
    ~EgServer() {}

    bool openSocket();
    void closeSocket();

    bool rcvRequest(std::string& theRequest);

    bool rcvByteArray(EgByteArrayAbstractType& byteArray);
    bool sendByteArray(EgByteArrayAbstractType& byteArray);

    bool rcvDataNode(EgDataNode& theNode);
    bool sendDataNode(EgDataNode& theNode);
};