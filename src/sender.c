#include "../includes/traceroute.h"

int send_message(t_traceroute_client* client, struct sockaddr_in sockaddr)
{
		t_icmp_packet packet;
		int           payload_size = 0;

		payload_size = build_echo_request(client, &packet);
		if (payload_size == ERROR)
		{
				client->status = EXIT_FAILURE;
				exit_program(client);
		}

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