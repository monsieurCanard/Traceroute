#include "../includes/traceroute.h"

void print_start(t_traceroute_client* client)
{
	//TODO Magic Number on max hops
    printf("traceroute to %s (%s), %i hops max, %li byte packets\n", client->name, client->ip, 30, sizeof(struct iphdr) + sizeof(struct icmphdr) + PAYLOAD_SIZE);
}