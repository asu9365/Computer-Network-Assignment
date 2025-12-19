// chat_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int sockfd;
pthread_t recv_thread;
int running = 1;

void *receive_handler(void *arg) {
    char buffer[BUF_SIZE];
    int read_size;

    while (running) {
        read_size = recv(sockfd, buffer, BUF_SIZE - 1, 0);
        if (read_size <= 0) {
            printf("Disconnected from server.\n");
            running = 0;
            break;
        }
        buffer[read_size] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    char message[BUF_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("Socket created.\n");

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    printf("Connected to server %s:%d\n", server_ip, port);
    printf("Type messages and press Enter to send. Type /quit to exit.\n");

    // Create thread to receive messages
    if (pthread_create(&recv_thread, NULL, receive_handler, NULL) < 0) {
        perror("Could not create receive thread");
        close(sockfd);
        return 1;
    }

    // Main loop: read from stdin and send to server
    while (running && fgets(message, BUF_SIZE, stdin) != NULL) {
        if (strncmp(message, "/quit", 5) == 0) {
            running = 0;
            break;
        }

        if (send(sockfd, message, strlen(message), 0) < 0) {
            perror("send failed");
            break;
        }
    }

    // Clean up
    running = 0;
    pthread_cancel(recv_thread);   // stop receiver
    pthread_join(recv_thread, NULL);
    close(sockfd);
    return 0;
}
