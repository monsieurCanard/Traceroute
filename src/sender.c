#include "../includes/traceroute.h"

int send_message(t_traceroute_client* client, struct sockaddr_in sockaddr)
{
		t_icmp_packet packet = {0};
		int           payload_size = sizeof(struct icmphdr) + PAYLOAD_SIZE;

		build_echo_request(client, &packet);

		if (sendto(client->fd,
							 &packet,
							 payload_size,
							 0,
							 (struct sockaddr*)&sockaddr,
							 sizeof(sockaddr)) == ERROR)
		{
				perror("Sendto error: ");
				return ERROR;
		}
		
		client->seq++;
		return SUCCESS;
}