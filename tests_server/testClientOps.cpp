#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create a socket
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // 2. Configure server address structure
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // Server port number

    // Convert IPv4 address from text to binary format
    // "127.0.0.1" is the standard localhost address
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported." << std::endl;
        close(client_socket);
        return -1;
    }

    // 3. Connect to the server
    std::cout << "Connecting to server..." << std::endl;
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed. Is the server running?" << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "Connected successfully!" << std::endl;

    // 4. Send data to the server
    std::string message = "1234";
    if (send(client_socket, message.c_str(), message.length(), 0) < 0) {
        std::cerr << "Failed to send data." << std::endl;
        close(client_socket);
        return -1;
    }
    std::cout << "Message sent: " << message << std::endl;

    // 5. Receive a response from the server
    char buffer[1024] = {0};
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        std::cerr << "Error while receiving data." << std::endl;
    } else if (bytes_received == 0) {
        std::cout << "Server closed the connection." << std::endl;
    } else {
        std::cout << "Server response: " << buffer << std::endl;
    }

    // 6. Close the socket
    close(client_socket);
    return 0;
}