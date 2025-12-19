# Computer Networks Laboratory Assignments

This repository contains the implementation of **Computer Networks Lab assignments** covering **TCP/UDP socket programming, Mininet experiments, packet capturing, RAW sockets, and network attack analysis**.  
All programs are written in **C** and executed in a **Linux + Mininet** environment.

---

## 🔧 Lab Setup & Tools

- **Operating System**: Linux (Ubuntu preferred)
- **Network Emulator**: Mininet
- **Programming Language**: C
- **Compiler**: gcc
- **Packet Analyzer**: Wireshark / Tshark
- **Traffic Generators**: Iperf, D-ITG
- **Libraries Used**:
  - socket.h
  - netinet/in.h
  - arpa/inet.h
  - pthread.h
  - math.h
  - time.h

---

## 📁 Directory Structure

```text
CN-Lab/
│
├── Assignment-01-TCP-Basics/
│   ├── server.c
│   └── client.c
│
├── Assignment-02-TCP-Fruit-Store/
│   ├── server_tcp.c
│   └── client_tcp.c
│
├── Assignment-03-UDP-Fruit-Store/
│   ├── server_udp.c
│   └── client_udp.c
│
├── Assignment-04-Wireshark-PING/
│   ├── ping_capture.pcap
│   └── analysis.txt
│
├── Assignment-05-Traffic-Generation/
│   ├── iperf_results.txt
│   └── ditg_results.txt
│
├── Assignment-06-TCP-Traffic-Analyzer/
│   └── tcp_analyzer.c
│
├── Assignment-07-UDP-Calculator/
│   ├── calc_server.c
│   └── calc_client.c
│
├── Assignment-08-TCP-Chat-Server/
│   ├── chat_server.c
│   ├── chat_client.c
│   └── log.txt
│
├── Assignment-09-TCP-File-Transfer/
│   ├── file_server.c
│   └── file_client.c
│
├── Assignment-10-RAW-Socket-Packets/
│   ├── raw_tcp.c
│   └── icmp_timestamp.c
│
├── Assignment-11-SYN-Flood/
│   └── syn_flood.c
│
├── Assignment-12-ICMP-Flood/
│   └── icmp_flood.c
│
├── Assignment-13-Packet-Analysis/
│   ├── packet_parser.c
│   └── ping_capture.pcapng
│
├── Assignment-14-Mininet-Topology/
│   └── leaf_spine_topology.py
│
└── README.md
