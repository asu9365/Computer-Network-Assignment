#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8081
#define MAX_FRUITS 10
#define MAX_CLIENTS 100

typedef struct {
    char name[32];
    int quantity;
    time_t last_sold;
} Fruit;

typedef struct {
    char ip[INET_ADDRSTRLEN];
    int port;
} Customer;

Fruit fruits[MAX_FRUITS];
int fruit_count = 0;

Customer customers[MAX_CLIENTS];
int customer_count = 0;

void init_fruits() {
    strcpy(fruits[0].name, "apple");
    fruits[0].quantity = 10;
    fruits[0].last_sold = 0;

    strcpy(fruits[1].name, "banana");
    fruits[1].quantity = 15;
    fruits[1].last_sold = 0;

    strcpy(fruits[2].name, "mango");
    fruits[2].quantity = 8;
    fruits[2].last_sold = 0;

    fruit_count = 3;
}

int find_fruit_index(const char *name) {
    for (int i = 0; i < fruit_count; i++) {
        if (strcasecmp(fruits[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_or_update_customer(const char *ip, int port) {
    for (int i = 0; i < customer_count; i++) {
        if (strcmp(customers[i].ip, ip) == 0 && customers[i].port == port)
            return; // already present
    }
    if (customer_count < MAX_CLIENTS) {
        strcpy(customers[customer_count].ip, ip);
        customers[customer_count].port = port;
        customer_count++;
    }
}

void format_time(time_t t, char *buf, size_t size) {
    if (t == 0) {
        snprintf(buf, size, "never");
        return;
    }
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

void build_stock_info(char *buffer, size_t size) {
    char line[128];
    buffer[0] = '\0';
    strcat(buffer, "\nCurrent Stock:\n");
    for (int i = 0; i < fruit_count; i++) {
        char timebuf[64];
        format_time(fruits[i].last_sold, timebuf, sizeof(timebuf));
        snprintf(line, sizeof(line), "%s: %d (last sold: %s)\n",
                 fruits[i].name, fruits[i].quantity, timebuf);
        strncat(buffer, line, size - strlen(buffer) - 1);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    char reply[2048];

    socklen_t addr_len = sizeof(client_addr);

    init_fruits();

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Fruit server started on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';

        // Client address
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);

        printf("\nRequest from %s:%d -> %s\n", client_ip, client_port, buffer);

        char fruit_name[32];
        int qty;
        int parsed = sscanf(buffer, "%s %d", fruit_name, &qty);

        reply[0] = '\0';

        if (parsed != 2 || qty <= 0) {
            snprintf(reply, sizeof(reply),
                     "Invalid request.\nUse: <fruit_name> <quantity>\n");
        } else {
            int idx = find_fruit_index(fruit_name);
            if (idx == -1) {
                snprintf(reply, sizeof(reply),
                         "Sorry, %s is not available.\n"
                         "Total unique customers so far: %d\n",
                         fruit_name, customer_count);
            } else {
                if (fruits[idx].quantity >= qty) {
                    fruits[idx].quantity -= qty;
                    fruits[idx].last_sold = time(NULL);

                    add_or_update_customer(client_ip, client_port);

                    snprintf(reply, sizeof(reply),
                             "Purchase successful: %d %s(s) sold.\n"
                             "Total unique customers so far: %d\n",
                             qty, fruits[idx].name, customer_count);
                } else {
                    snprintf(reply, sizeof(reply),
                             "Regret! Only %d %s(s) available. Cannot sell %d.\n"
                             "Total unique customers so far: %d\n",
                             fruits[idx].quantity, fruits[idx].name, qty,
                             customer_count);
                }
            }
        }

        char stock_info[1024];
        build_stock_info(stock_info, sizeof(stock_info));
        strncat(reply, stock_info, sizeof(reply) - strlen(reply) - 1);

        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}

