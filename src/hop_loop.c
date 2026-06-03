#include "../includes/traceroute.h"

extern bool g_exit_program;

void hop_loop(t_traceroute_client* client)
{
	for(int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
	{
		gettimeofday(&client->hop_packet.send_time[attempt], NULL);

		if (send_message(client, client->sockaddr) == ERROR)
		{
			client->status = EXIT_FAILURE;
			exit_program(client);
		}
		recv_loop(client, attempt);
	}
}