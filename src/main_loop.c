#include "../includes/traceroute.h"
#include <netdb.h>

extern bool g_exit_program;

static void print_hop_result(t_traceroute_client client)
{
	printf("%2d  ", client.current_hop);
	for (int i = 0; i < MAX_ATTEMPTS; i++)
	{
		if (client.hop_packet.rtt[i] < 0)
		{
			printf("*   ");
			continue;
		}
		if (i == 0 || strcmp(client.hop_packet.hosts[i], client.hop_packet.hosts[i - 1]) != 0	)
		{
			printf("%s (%s) ", client.hop_packet.hosts[i], client.hop_packet.ips[i]);
		}
		
		printf("%.3f ms  ",client.hop_packet.rtt[i]);
	}
	printf("\n");
}


void main_loop(t_traceroute_client* client)
{
	while (!g_exit_program && client->current_hop <= MAX_HOPS)
	{
		client->current_hop++;

		int ret = setsockopt(client->fd, IPPROTO_IP, IP_TTL, &client->current_hop, sizeof(client->current_hop));
		
		if (ret < 0)
		{
			perror("setsockopt error: ");
			client->status = EXIT_FAILURE;
			exit_program(client);
		}
		hop_loop(client);
		print_hop_result(*client);
	}
	exit_program(client);
}