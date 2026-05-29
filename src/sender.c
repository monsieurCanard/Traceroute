#include "../includes/traceroute.h"

int send_message(t_traceroute_client* client, struct sockaddr_in sockaddr)
{
		printf("Sending message\n");
		t_icmp_packet packet;
		int           payload_size = 0;

		// client->counter.transmitted++;
		printf("Building echo request\n");

		payload_size = build_echo_request(client, &packet);
		if (payload_size == ERROR)
		{
				client->status = EXIT_FAILURE;
				exit_program(client);
		}
		printf("Echo request built with size %d\n", payload_size);
		if (sendto(client->fd,
							 &packet,
							 payload_size,
							 0,
							 (struct sockaddr*)&sockaddr,
							 sizeof(sockaddr)) == ERROR)
		{
				perror("Sendto error: ");
				return EXIT_FAILURE;
		}
		client->seq++;
		
		return EXIT_SUCCESS;
		// client->packets[client->seq % MAX_PING_SAVES].receive = false;
}