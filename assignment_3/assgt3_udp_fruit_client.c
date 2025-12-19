#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8081

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[256], server_reply[2048];

    if (argc < 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // e.g. 10.0.0.1

    printf("UDP Client started. Server: %s:%d\n", argv[1], PORT);
    printf("Enter: <fruit_name> <quantity>\n");
    printf("Type 'exit' to quit.\n\n");

    socklen_t addr_len = sizeof(server_addr);

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

        if (sendto(sockfd, message, strlen(message), 0,
                   (struct sockaddr *)&server_addr, addr_len) < 0) {
            perror("sendto failed");
            break;
        }

        memset(server_reply, 0, sizeof(server_reply));
        int n = recvfrom(sockfd, server_reply, sizeof(server_reply) - 1, 0,
                         (struct sockaddr *)&server_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            break;
        }
        server_reply[n] = '\0';

        printf("\n--- Server Response ---\n%s\n", server_reply);
    }

    close(sockfd);
    return 0;
}

