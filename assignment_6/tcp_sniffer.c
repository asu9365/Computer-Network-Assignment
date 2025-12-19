#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>

int main() {
    int sockfd;
    unsigned char buffer[65536];
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    // Create RAW socket to capture TCP packets
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
        perror("Socket Error");
        return 1;
    }
    printf("TCP Packet Sniffer Started...\n");

    while (1) {
        // Receive packets
        int buflen = recvfrom(sockfd, buffer, sizeof(buffer), 0, &saddr, (socklen_t *)&saddr_len);
        if (buflen < 0) {
            perror("Recvfrom Error");
            return 1;
        }

        // Extract Ethernet header = first 14 bytes
        struct ethhdr *eth = (struct ethhdr *)buffer;

        printf("\n===== Ethernet Header =====\n");
        printf("Destination MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
               eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
        printf("Source MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
               eth->h_source[0], eth->h_source[1], eth->h_source[2],
               eth->h_source[3], eth->h_source[4], eth->h_source[5]);

        // IP header starts right after Ethernet header
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        struct sockaddr_in src, dst;
        src.sin_addr.s_addr = ip->saddr;
        dst.sin_addr.s_addr = ip->daddr;

        printf("\n===== IP Header =====\n");
        printf("Source IP: %s\n", inet_ntoa(src.sin_addr));
        printf("Destination IP: %s\n", inet_ntoa(dst.sin_addr));
        printf("Protocol: %d\n", ip->protocol);

        // Check if TCP protocol
        if (ip->protocol == 6) {
            struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + ip->ihl * 4);

            printf("\n===== TCP Header =====\n");
            printf("Source Port: %u\n", ntohs(tcp->source));
            printf("Destination Port: %u\n", ntohs(tcp->dest));
            printf("Sequence Number: %u\n", ntohl(tcp->seq));
            printf("Acknowledgment: %u\n", ntohl(tcp->ack_seq));

            int header_size = sizeof(struct ethhdr) + ip->ihl * 4 + tcp->doff * 4;
            int payload_len = buflen - header_size;

            printf("TCP Payload Size: %d bytes\n", payload_len);
        }
    }
    close(sockfd);
    return 0;
}
