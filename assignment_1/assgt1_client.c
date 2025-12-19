#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server;
    char server_reply[50];
    char msg[] = "Hi";

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("10.0.0.1");   // Server host IP

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    send(sockfd, msg, strlen(msg), 0);

    recv(sockfd, server_reply, sizeof(server_reply), 0);
    printf("Server replied: %s\n", server_reply);

    close(sockfd);
    return 0;
}

