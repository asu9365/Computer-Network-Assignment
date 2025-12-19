#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd, new_sock;
    struct sockaddr_in server, client;
    char client_msg[50], server_msg[50] = "Hello";

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Socket creation failed\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        return 1;
    }
    printf("Server started...\n");

    // Listen
    listen(sockfd, 3);

    socklen_t c = sizeof(client);

    // Accept connection
    new_sock = accept(sockfd, (struct sockaddr*)&client, &c);

    recv(new_sock, client_msg, sizeof(client_msg), 0);
    printf("Client says: %s\n", client_msg);

    send(new_sock, server_msg, strlen(server_msg), 0);

    close(new_sock);
    close(sockfd);
    return 0;
}

