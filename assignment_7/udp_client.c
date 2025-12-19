// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#define BUF_SIZE 512
#define MAX_RETRIES 3

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Set a receive timeout (e.g., 2 seconds) for packet-loss detection
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Scientific Calculator Client\n");
    printf("Server: %s:%d\n", server_ip, server_port);

    unsigned int seq = 1;
    while (1) {
        printf("\nEnter expression (e.g., sin(1.5708), 3+4, 10/3, inv(5)) or 'quit':\n> ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break;
        }

        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, "quit") == 0 || strcmp(buffer, "exit") == 0) {
            break;
        }

        // Build message: "SEQ EXPR"
        char sendbuf[BUF_SIZE];
        snprintf(sendbuf, sizeof(sendbuf), "%u %s", seq, buffer);

        int retries = 0;
        int got_reply = 0;

        while (retries < MAX_RETRIES && !got_reply) {
            ssize_t sent = sendto(sockfd, sendbuf, strlen(sendbuf), 0,
                                  (const struct sockaddr *)&servaddr, sizeof(servaddr));
            if (sent < 0) {
                perror("sendto");
                break;
            }

            printf("Sent (seq=%u): %s\n", seq, buffer);

            // Wait for reply
            struct sockaddr_in fromaddr;
            socklen_t fromlen = sizeof(fromaddr);
            char recvbuf[BUF_SIZE];
            ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf) - 1, 0,
                                 (struct sockaddr *)&fromaddr, &fromlen);

            if (n < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Timeout -> possible packet loss
                    retries++;
                    printf("Timeout waiting for response (attempt %d). Possible packet loss.\n",
                           retries);
                    if (retries < MAX_RETRIES) {
                        printf("Retransmitting...\n");
                    }
                    continue;
                } else {
                    perror("recvfrom");
                    break;
                }
            }

            recvbuf[n] = '\0';

            // Parse "SEQ RESULT"
            unsigned int rseq = 0;
            char *space = strchr(recvbuf, ' ');
            if (!space) {
                printf("Malformed reply: %s\n", recvbuf);
                continue;
            }
            *space = '\0';
            rseq = (unsigned int)strtoul(recvbuf, NULL, 10);
            char *rest = space + 1;

            if (rseq != seq) {
                printf("Received out-of-order or duplicate packet (expected seq=%u, got seq=%u).\n",
                       seq, rseq);
                // Ignore and wait again (until timeout or correct seq)
                continue;
            }

            if (strncmp(rest, "ERR", 3) == 0) {
                printf("Server reported an error evaluating expression.\n");
            } else {
                double val = atof(rest);
                printf("Result for seq=%u: %lf\n", rseq, val);
            }

            got_reply = 1;
        }

        if (!got_reply) {
            printf("No response from server after %d attempts. Giving up on seq=%u.\n",
                   MAX_RETRIES, seq);
        }

        seq++;
    }

    close(sockfd);
    return 0;
}
