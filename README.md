# COMPUTER NETWORKS LAB (CN LAB)

This repository contains the complete implementation of **Computer Networks Laboratory assignments** using **C programming, Mininet, Wireshark, and RAW sockets**.  
Each assignment section includes the **official question (Objective, Exercise, Steps/Hints, and Learning Outcomes)** along with the implemented solution.

---

## LAB ENVIRONMENT

- **Operating System**: Linux (Ubuntu)
- **Network Emulator**: Mininet
- **Programming Language**: C
- **Compiler**: GCC
- **Packet Capture Tool**: Wireshark / Tshark
- **Traffic Generation Tools**: Iperf, D-ITG

---

## REPOSITORY STRUCTURE

```text
CN-Lab/
├── Assignment-01/
├── Assignment-02/
├── Assignment-03/
├── Assignment-04/
├── Assignment-05/
├── Assignment-06/
├── Assignment-07/
├── Assignment-08/
├── Assignment-09/
├── Assignment-10/
├── Assignment-11/
├── Assignment-12/
├── Assignment-13/
├── Assignment-14/
└── README.md

```


---

# 📘 Assignment Questions

---

## Assignment 1 – TCP Client and Server

### Objective
To gain first hands-on experience of basic socket programming.

### Exercise
Write a program to run TCP client and server socket programs where the client first says **“Hi”** and in response the server says **“Hello”**.

### Steps / Hints
- Create two Mininet hosts  
- Open the hosts individually in xterm windows  
- Run TCP server on one host and TCP client on the other  

### Learning Outcomes
- Basics of TCP client and server programming  

---

## Assignment 2 – TCP Socket Programming (Fruit Store)

### Objective
To gain experience of TCP socket programming for simple applications.

### Exercise
Write a program using TCP socket to implement the following:
- Server maintains records of fruits in the format:  
  `fruit-name, quantity, last-sold (server timestamp)`
- Multiple clients purchase fruits one at a time  
- Fruit quantity is updated after every sale  
- Send regret message if requested quantity is not available  
- Display customer IDs `<IP, Port>` who performed transactions  
- Display total number of unique customers to each client  

### Steps / Hints
- Use at least two Mininet hosts as clients  
- Server must run continuously using a loop  
- Use a new socket from `accept()` to track client information  
- Server sends current stock information to the client  

### Learning Outcomes
- Multi-client communication using TCP sockets  

---

## Assignment 3 – UDP Socket Programming

### Objective
To gain experience of UDP socket programming for simple applications.

### Exercise
Redo **Assignment 2** using **UDP sockets**.

### Steps / Hints
- Same as Assignment 1  

### Learning Outcomes
- Basics of UDP socket programming  

---

## Assignment 4 – Packet Capturing using Wireshark

### Objective
To gain knowledge of packet capturing tools and understand header structures.

### Exercise
Install Wireshark and capture packets during a **PING** operation.  
Draw a time diagram and list **L2, L3, and L4 headers** from the captured file.

### Steps / Hints
- Install Wireshark in Linux VM with Mininet  
- Start capturing on any interface  
- Ping between two hosts  
- Analyze the captured packets  

### Learning Outcomes
- Knowledge of packet capturing tools and protocol headers  

---

## Assignment 5 – Packet Generation Tools

### Objective
To gain knowledge of packet generation tools.

### Exercise
Install and use **Iperf**, **D-ITG**, etc., to generate traffic among Mininet hosts.

### Learning Outcomes
- Knowledge of packet generation and analysis tools  

---

## Assignment 6 – TCP Traffic Analysis

### Objective
To gain knowledge of TCP/IP C libraries.

### Exercise
Develop a simple C-based network simulator to analyze TCP traffic.

### Steps / Hints
- Use socket libraries to listen to incoming packets  
- Extract headers and payload  

### Learning Outcomes
- Knowledge of TCP/IP libraries  

---

## Assignment 7 – UDP Scientific Calculator

### Objective
Client–server communication using UDP packets.

### Exercise
Client sends numbers and operations (`sin`, `cos`, `+`, `−`, `*`, `/`).  
Server evaluates and returns the result.  
Detect packet loss using Wireshark.

### Learning Outcomes
- Understanding reliability issues in UDP  

---

## Assignment 8 – Multi-threaded TCP Chat Server

### Objective
Using thread library and TCP sockets.

### Exercise
Implement a group chat server where multiple clients communicate through the server.  
Each client is handled by a separate thread and logs are maintained.

### Learning Outcomes
- Understanding multi-threaded TCP servers  

---

## Assignment 9 – TCP File Upload and Download

### Objective
File transfer using TCP.

### Exercise
Client downloads a file from the server and uploads a file to the server.  
Calculate transfer time in both cases.

### Learning Outcomes
- Use of TCP for file transfer  

---

## Assignment 10 – RAW Socket Packet Generation

### Objective
Using RAW sockets to generate packets.

### Exercise
- Generate TCP packet with roll number as payload  
- Generate ICMP timestamp messages  

### Learning Outcomes
- RAW socket programming and packet generation  

---

## Assignment 11 – TCP SYN Flood Attack ⚠️

### Objective
Using RAW sockets to generate TCP flooding attack.

### Exercise
Generate TCP SYN flood traffic using multiple Mininet hosts.

### Learning Outcomes
- Understanding TCP-based DDoS attacks  

---

## Assignment 12 – ICMP Flood Attack ⚠️

### Objective
Using RAW sockets to generate ICMP flooding attack.

### Exercise
Generate ICMP flood traffic using spoofed Mininet hosts.

### Learning Outcomes
- Understanding ICMP-based attacks  

---

## Assignment 13 – Packet Analysis in Binary Tree Topology

### Objective
To learn packet capturing and analysis.

### Exercise
Create a binary tree topology in Mininet, capture packets at the root switch, and analyze protocols during PING.

### Learning Outcomes
- Learning roles of L2/L3/L4 protocols  

---

## Assignment 14 – Leaf–Spine Topology

### Objective
Creating customized topologies in Mininet.

### Exercise
Create a scalable **leaf–spine topology** using Mininet Python API.

### Learning Outcomes
- Understanding Mininet API and virtual networks  

---


