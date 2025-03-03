#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>

using namespace std;

int main() {
    // 1. Create a socket (IPv4, TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    // 2. Define address and bind to port 9999
    struct sockaddr_in address = {
        AF_INET,            // IPv4
        htons(9999),        // Port number
        0                   // Bind to all available interfaces
    };

    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed!" << endl;
        close(sockfd);
        return 1;
    }

    // 3. Listen for incoming connections (max 10)
    if (listen(sockfd, 10) < 0) {
        cerr << "Listen failed!" << endl;
        close(sockfd);
        return 1;
    }

    // 4. Accept client connection
    int clientfd = accept(sockfd, nullptr, nullptr);
    if (clientfd < 0) {
        cerr << "Accept failed!" << endl;
        close(sockfd);
        return 1;
    }

    // 5. Set up polling for stdin and client socket
    struct pollfd fds[2] = {
        {0, POLLIN, 0},      // Monitor stdin for input
        {clientfd, POLLIN, 0} // Monitor client socket for data
    };

    // 6. Communication loop
    for (;;) {
        char buffer[256] = { 0 };

        // Poll for input on stdin or from client
        poll(fds, 2, 50000);  // Timeout of 50 seconds

        // If input from stdin, send to client
        if (fds[0].revents & POLLIN) {
            read(0, buffer, 255);  // Read input from stdin
            string message = "Server: " + string(buffer);  // Prefix with "Server:"
            send(clientfd, message.c_str(), message.length(), 0);  // Send message to client
        } 
        // If input from client, display it
        else if (fds[1].revents & POLLIN) {
            if (recv(clientfd, buffer, 255, 0) == 0) {  // Client disconnected
                cout << "Client disconnected." << endl;
                break;
            }
            printf("%s\n", buffer);  // Print client message
        }
    }

    // Clean up and close sockets
    close(clientfd);
    close(sockfd);

    return 0;
}

