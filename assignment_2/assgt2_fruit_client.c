#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[256], server_reply[2048];

    if (argc < 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(argv[1]); // e.g. 10.0.0.1

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        close(sock);
        return 1;
    }

    printf("Connected to Fruit Server %s:%d\n", argv[1], PORT);
    printf("Enter purchase request as: <fruit_name> <quantity>\n");
    printf("Example: apple 3\n");
    printf("Type 'exit' to quit.\n\n");

    while (1) {
        printf("Your request: ");
        fflush(stdout);

        if (fgets(message, sizeof(message), stdin) == NULL) {
            break;
        }

        // Remove newline
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            break;
        }

        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send failed");
            break;
        }

        memset(server_reply, 0, sizeof(server_reply));
        int bytes = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
        if (bytes <= 0) {
            printf("Server closed connection.\n");
            break;
        }

        printf("\n--- Server Response ---\n%s\n", server_reply);
    }

    close(sock);
    return 0;
}

