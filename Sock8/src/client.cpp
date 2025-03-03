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

    // 2. Define server address and connect to port 9999
    struct sockaddr_in address = {
        AF_INET,            // IPv4
        htons(9999),        // Server port
        inet_addr("127.0.0.1")  // Server IP (localhost)
    };

    // 3. Connect to the server
    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Connection failed!" << endl;
        close(sockfd);
        return 1;
    }

    // 4. Set up polling for stdin and socket
    struct pollfd fds[2] = {
        {0, POLLIN, 0},    // Monitor stdin for input
        {sockfd, POLLIN, 0} // Monitor socket for data from server
    };

    // 5. Communication loop
    for (;;) {
        char buffer[256] = { 0 };

        // 6. Poll for input from stdin or server
        poll(fds, 2, 50000);  // Timeout of 50 seconds

        // If input from stdin, send to server
        if (fds[0].revents & POLLIN) {
            read(0, buffer, 255);  // Read from stdin
            string message = "Client: " + string(buffer);  // Prefix with "Client:"
            send(sockfd, message.c_str(), message.length(), 0);  // Send message to server
        } 
        // If input from server, display it
        else if (fds[1].revents & POLLIN) {
            if (recv(sockfd, buffer, 255, 0) == 0) {  // Server disconnected
                cout << "Server disconnected." << endl;
                break;
            }
            printf("%s\n", buffer);  // Print server message
        }
    }

    // Clean up and close socket
    close(sockfd);

    return 0;
}

