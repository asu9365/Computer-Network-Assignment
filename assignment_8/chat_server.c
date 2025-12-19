// chat_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_CLIENTS 100
#define BUF_SIZE 1024

int client_sockets[MAX_CLIENTS];
int client_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Get current timestamp as string "[YYYY-MM-DD HH:MM:SS]"
void get_timestamp(char *buf, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buf, size, "[%Y-%m-%d %H:%M:%S]", t);
}

// Append message to log.txt with timestamp
void log_message(const char *msg) {
    FILE *fp;
    char timebuf[64];

    get_timestamp(timebuf, sizeof(timebuf));

    pthread_mutex_lock(&log_mutex);
    fp = fopen("log.txt", "a");
    if (fp != NULL) {
        fprintf(fp, "%s %s\n", timebuf, msg);
        fclose(fp);
    } else {
        perror("fopen log.txt");
    }
    pthread_mutex_unlock(&log_mutex);
}

// Broadcast message to all connected clients
void broadcast_message(const char *msg, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++) {
        int sock = client_sockets[i];
        if (sock != sender_fd) {
            if (send(sock, msg, strlen(msg), 0) < 0) {
                perror("send");
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// Remove client from list
void remove_client(int sock) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == sock) {
            // Move last client into this slot
            client_sockets[i] = client_sockets[client_count - 1];
            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// Thread function for each client
void *client_handler(void *arg) {
    int sock = *(int *)arg;
    free(arg);

    char buffer[BUF_SIZE];
    int read_size;

    // Add this client to the global list
    pthread_mutex_lock(&clients_mutex);
    if (client_count < MAX_CLIENTS) {
        client_sockets[client_count++] = sock;
    } else {
        printf("Max clients reached. Rejecting client.\n");
        close(sock);
        pthread_mutex_unlock(&clients_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&clients_mutex);

    printf("Client connected (fd=%d)\n", sock);

    // Inform others
    snprintf(buffer, sizeof(buffer), "Client %d joined the chat.\n", sock);
    log_message(buffer);
    broadcast_message(buffer, sock);

    // Chat loop
    while ((read_size = recv(sock, buffer, BUF_SIZE - 1, 0)) > 0) {
        buffer[read_size] = '\0';

        // Prepare log line like: "[timestamp] (fd=5): message"
        char logbuf[BUF_SIZE + 64];
        char timebuf[64];
        get_timestamp(timebuf, sizeof(timebuf));
        snprintf(logbuf, sizeof(logbuf), "%s (fd=%d): %s", timebuf, sock, buffer);

        // Write to log (without double timestamp from log_message)
        pthread_mutex_lock(&log_mutex);
        FILE *fp = fopen("log.txt", "a");
        if (fp != NULL) {
            fprintf(fp, "%s\n", logbuf);
            fclose(fp);
        } else {
            perror("fopen log.txt");
        }
        pthread_mutex_unlock(&log_mutex);

        // Broadcast to other clients
        broadcast_message(logbuf, sock);
    }

    if (read_size == 0) {
        printf("Client disconnected (fd=%d)\n", sock);
    } else if (read_size == -1) {
        perror("recv failed");
    }

    // Remove client and close socket
    remove_client(sock);
    close(sock);

    // Inform others
    snprintf(buffer, sizeof(buffer), "Client %d left the chat.\n", sock);
    log_message(buffer);
    broadcast_message(buffer, sock);

    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, newfd, port;
    struct sockaddr_in server, client;
    socklen_t c;
    pthread_t thread_id;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Bind
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }
    printf("Bind done on port %d.\n", port);

    // Listen
    if (listen(sockfd, 10) < 0) {
        perror("Listen failed");
        close(sockfd);
        return 1;
    }
    printf("Waiting for incoming connections...\n");

    c = sizeof(struct sockaddr_in);
    while ((newfd = accept(sockfd, (struct sockaddr *)&client, &c))) {
        printf("Accepted connection from %s:%d, fd=%d\n",
               inet_ntoa(client.sin_addr), ntohs(client.sin_port), newfd);

        int *new_sock = malloc(sizeof(int));
        if (new_sock == NULL) {
            perror("malloc");
            close(newfd);
            continue;
        }
        *new_sock = newfd;

        if (pthread_create(&thread_id, NULL, client_handler, (void *)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            close(newfd);
            continue;
        }

        // Detach thread so resources are freed automatically
        pthread_detach(thread_id);
    }

    if (newfd < 0) {
        perror("accept failed");
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return 0;
}
