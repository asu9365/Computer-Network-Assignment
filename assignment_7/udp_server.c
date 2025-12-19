// udp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 9000
#define BUF_SIZE 512

// trim leading/trailing spaces
void trim(char *s) {
    char *p = s;
    int l = strlen(p);

    // trim end
    while (l > 0 && (p[l-1] == ' ' || p[l-1] == '\n' || p[l-1] == '\t')) {
        p[--l] = '\0';
    }
    // trim start
    while (*p == ' ' || *p == '\t')
        p++;

    if (p != s)
        memmove(s, p, strlen(p) + 1);
}

// Evaluate very simple expressions: sin(x), cos(x), tan(x), inv(x), a+b, a-b, a*b, a/b
// Returns 0 on success, -1 on error.
int evaluate_expression(const char *expr_in, double *result) {
    char expr[256];
    strncpy(expr, expr_in, sizeof(expr) - 1);
    expr[sizeof(expr) - 1] = '\0';
    trim(expr);

    if (strlen(expr) == 0)
        return -1;

    // Check for unary functions: sin(x), cos(x), tan(x), inv(x)
    if (strncmp(expr, "sin(", 4) == 0) {
        char *end = strrchr(expr, ')');
        if (!end) return -1;
        *end = '\0';
        double x = atof(expr + 4);
        *result = sin(x);
        return 0;
    } else if (strncmp(expr, "cos(", 4) == 0) {
        char *end = strrchr(expr, ')');
        if (!end) return -1;
        *end = '\0';
        double x = atof(expr + 4);
        *result = cos(x);
        return 0;
    } else if (strncmp(expr, "tan(", 4) == 0) {
        char *end = strrchr(expr, ')');
        if (!end) return -1;
        *end = '\0';
        double x = atof(expr + 4);
        *result = tan(x);
        return 0;
    } else if (strncmp(expr, "inv(", 4) == 0) {
        char *end = strrchr(expr, ')');
        if (!end) return -1;
        *end = '\0';
        double x = atof(expr + 4);
        if (x == 0.0) return -1;
        *result = 1.0 / x;
        return 0;
    }

    // Binary operators: a+b, a-b, a*b, a/b
    // Find operator position (skip first char to avoid unary minus)
    char *op = NULL;
    for (size_t i = 1; i < strlen(expr); i++) {
        if (expr[i] == '+' || expr[i] == '-' ||
            expr[i] == '*' || expr[i] == '/') {
            op = &expr[i];
            break;
        }
    }
    if (!op) {
        return -1;
    }

    char op_char = *op;
    *op = '\0';
    char *left_str = expr;
    char *right_str = op + 1;
    trim(left_str);
    trim(right_str);

    if (strlen(left_str) == 0 || strlen(right_str) == 0)
        return -1;

    double a = atof(left_str);
    double b = atof(right_str);

    switch (op_char) {
        case '+': *result = a + b; break;
        case '-': *result = a - b; break;
        case '*': *result = a * b; break;
        case '/':
            if (b == 0.0) return -1;
            *result = a / b;
            break;
        default:
            return -1;
    }

    return 0;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Fill server information
    servaddr.sin_family = AF_INET;       // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Scientific Calculator Server listening on port %d...\n", SERVER_PORT);

    socklen_t len;
    int n;

    while (1) {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, (char *)buffer, BUF_SIZE - 1, 0,
                     (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[n] = '\0';

        // Parse "SEQ EXPR"
        unsigned int seq = 0;
        char expr[256] = {0};

        char *first_space = strchr(buffer, ' ');
        if (!first_space) {
            fprintf(stderr, "Invalid request: %s\n", buffer);
            continue;
        }

        *first_space = '\0';
        seq = (unsigned int)strtoul(buffer, NULL, 10);
        strncpy(expr, first_space + 1, sizeof(expr) - 1);
        expr[sizeof(expr) - 1] = '\0';
        trim(expr);

        printf("Received from client: seq=%u expr=\"%s\"\n", seq, expr);

        double result;
        int status = evaluate_expression(expr, &result);

        char reply[BUF_SIZE];
        if (status == 0) {
            snprintf(reply, sizeof(reply), "%u %lf", seq, result);
        } else {
            snprintf(reply, sizeof(reply), "%u ERR", seq);
        }

        sendto(sockfd, reply, strlen(reply), 0,
               (const struct sockaddr *)&cliaddr, len);
        printf("Sent to client: %s\n", reply);
    }

    close(sockfd);
    return 0;
}
