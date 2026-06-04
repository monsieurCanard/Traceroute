#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/** 
 * Constants and Configurations 
 */
#define PAYLOAD_SIZE 32
#define MAX_HOPS 100
#define MAX_ATTEMPTS 3

#define TIMEOUT_SEC 1
#define TIMEOUT_USEC 0

#define MSG_TIMEOUT -1

#define ERROR -1
#define SUCCESS 0

/**
 * @struct data_icmp
 * @brief Represents an ICMP packet containing the IP header and ICMP data.
 */
typedef struct data_icmp
{
    struct iphdr*   ip_header;
    struct icmphdr* data;
} t_data_icmp;

/**
 * @struct icmp_packet
 * @brief Represents custom ICMP packet structure containing the header and payload.
 */
typedef struct icmp_packet
{
    struct icmphdr hdr;             ///< Standard ICMP header
    char payload[PAYLOAD_SIZE];     ///< Packet payload padded with a specific size
} t_icmp_packet;

/**
 * @struct hop_packets
 * @brief Stores information regarding responses for each hop during traceroute.
 */
typedef struct hop_packets
{
    struct sockaddr_in src_addr[3];
    char ips[3][INET_ADDRSTRLEN];   ///< IP addresses for each attempt
    char hosts[3][1024];            ///< Resolved hostnames for each attempt
    double rtt[3];                  ///< Round trip time for each attempt (ms)
    struct timeval send_time[3];    ///< Timestamps of when packets were sent
} t_hop_packets;

/**
 * @struct traceroute_client
 * @brief Main state structure for the traceroute client, holding socket, target, and execution state.
 */
typedef struct traceroute_client
{
    struct sockaddr_in sockaddr;    ///< Target socket address
    t_icmp_packet*     packets;     ///< Pointer to list of packets to send
    fd_set             read_fds;    ///< File descriptor set for `select()`
    
    t_hop_packets      hop_packet[MAX_HOPS];  ///< State data for the current hop
    
    int current_hop;                ///< TTL/current hop index starting from 1
    unsigned char recv_buff[1024];  ///< Buffer for receiving response packets
    struct timeval now;             ///< Current time reference
    
    char* name;                     ///< Initial hostname or command-line target arg
    int   fd;                       ///< Raw socket file descriptor
    char* ip;                       ///< Resolved IP address string
    int   seq;                      ///< Current sequence number for ICMP packets
    int   status;                   ///< Client execution status or error code
    bool  target_reached;           ///< Flag to stop traceroute when destination is reached
} t_traceroute_client;

/**
 * @name Parser Functions
 */
int parse_args(int ac, char** av);

/**
 * @name Core Traceroute Logic
 * @brief Client initialization, network interaction, and loops
 */

/**
 * @brief Extract information like IP and hostname from the received packet.
 */
void get_info_from_response(t_traceroute_client* client, struct sockaddr_in src_addr, int attempt);

/**
 * @brief Initialize traceroute client socket and target resolution.
 */
int create_client(t_traceroute_client* client, char* address);

/**
 * @brief Prepare and populate an ICMP Echo Request packet.
 */
void build_echo_request(t_traceroute_client* client, t_icmp_packet* packet);

/**
 * @brief Calculate the ICMP checksum for a packet array of bytes.
 */
int icmp_checksum(unsigned char* buff, int len);

/**
 * @name Main Loops
 */
void main_loop(t_traceroute_client* client);
void hop_loop(t_traceroute_client* client);
void recv_loop(t_traceroute_client* client, int attempt);

/**
 * @brief Send an ICMP packet to the specified sockaddr.
 */
int send_message(t_traceroute_client* client, struct sockaddr_in sockaddr);

/**
 * @brief Verify that the received socket response targets our ongoing traceroute logic.
 */
int verify_response(t_traceroute_client* client, struct sockaddr_in src_addr);

/**
 * @name Lifecycle Functions
 */
void exit_program(t_traceroute_client* client);
void print_start(t_traceroute_client client);

/**
 * @brief Verify if the time elapsed since sending a packet exceeds the defined timeout.
 */
bool msg_is_timeout(struct timeval send_time, struct timeval now);

#endif /* TRACEROUTE_H */
