#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../nodes/egDataNode.h"
#include "../server/egLiteralsServer.h"

class EgClient { // "type" means c++ type, data metatype called "blueprint"
public:
    int serverSocket { 0 };
    int clientSocket { 0 };
    int serverPort   { defaultServerPort };
    int opt          { 1 }; // setsockopt aux data

    EgClient() {}
    ~EgClient() {}

    bool openSocket();
    void closeSocket();

    bool rcvRequest(std::string& theRequest);

    bool rcvDataNode(EgDataNode& theNode);
    bool sendDataNode(EgDataNode& theNode);
};