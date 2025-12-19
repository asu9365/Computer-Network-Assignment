#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_FRUITS 10
#define MAX_CLIENTS 100

typedef struct {
    char name[32];
    int quantity;
    time_t last_sold; // 0 means never sold
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
    // Initial stock (you can edit as needed)
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
        if (strcmp(customers[i].ip, ip) == 0 && customers[i].port == port) {
            return; // already present
        }
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
    int server_fd, new_sock;
    struct sockaddr_in address, client_addr;
    socklen_t addrlen = sizeof(address);
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    init_fruits();

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // listen on all interfaces
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Fruit server started on port %d...\n", PORT);

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\nWaiting for client connection...\n");
        new_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (new_sock < 0) {
            perror("accept failed");
            continue;
        }

        // Get client IP & port
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);

        printf("Connected to client: %s:%d\n", client_ip, client_port);

        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(new_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            close(new_sock);
            continue;
        }

        printf("Request from client %s:%d -> %s\n", client_ip, client_port, buffer);

        // Expected format: fruit_name quantity
        char fruit_name[32];
        int qty;
        int parsed = sscanf(buffer, "%s %d", fruit_name, &qty);

        char reply[2048];
        reply[0] = '\0';

        if (parsed != 2 || qty <= 0) {
            snprintf(reply, sizeof(reply),
                     "Invalid request format.\nUse: <fruit_name> <quantity>\n");
        } else {
            int idx = find_fruit_index(fruit_name);
            if (idx == -1) {
                snprintf(reply, sizeof(reply),
                         "Sorry, %s is not available in the store.\n", fruit_name);
            } else {
                if (fruits[idx].quantity >= qty) {
                    // Successful transaction
                    fruits[idx].quantity -= qty;
                    fruits[idx].last_sold = time(NULL);

                    add_or_update_customer(client_ip, client_port);

                    snprintf(reply, sizeof(reply),
                             "Purchase successful: %d %s(s) sold.\n"
                             "Total unique customers so far: %d\n",
                             qty, fruits[idx].name, customer_count);
                } else {
                    // Regret message
                    snprintf(reply, sizeof(reply),
                             "Regret! Only %d %s(s) are available. Cannot sell %d.\n"
                             "Total unique customers so far: %d\n",
                             fruits[idx].quantity, fruits[idx].name, qty, customer_count);
                }
            }
        }

        // Append stock info in all cases
        char stock_info[1024];
        build_stock_info(stock_info, sizeof(stock_info));
        strncat(reply, stock_info, sizeof(reply) - strlen(reply) - 1);

        send(new_sock, reply, strlen(reply), 0);

        close(new_sock);
    }

    close(server_fd);
    return 0;
}

