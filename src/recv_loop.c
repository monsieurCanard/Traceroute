#include "../includes/traceroute.h"

extern bool g_exit_program;

void recv_loop(t_traceroute_client* client, int attempt)
{
	while(!g_exit_program)
	{
		struct timeval timeout;

		timeout.tv_sec  = TIMEOUT_SEC;
		timeout.tv_usec = TIMEOUT_USEC;
		
		FD_ZERO(&client->read_fds);
		FD_SET(client->fd, &client->read_fds);
		
		int ret = select(client->fd + 1, &client->read_fds, NULL, NULL, &timeout);
		
		gettimeofday(&client->now, NULL);

		//TIMEOUT
		if (ret == 0)
		{
			if(msg_is_timeout(client->hop_packet[client->current_hop].send_time[attempt], client->now))
			{
				client->hop_packet[client->current_hop].rtt[attempt] = MSG_TIMEOUT;
				break;
			}
			continue;
		}

		//ERROR
		if (ret < 0)
		{
				if (errno != EINTR)
						client->status = EXIT_FAILURE;
				exit_program(client);
		}

		//RESPONSE RECEIVED
		if (ret == 1)
		{
			if (!FD_ISSET(client->fd, &client->read_fds))
					continue;

			struct sockaddr_in src_addr;
			socklen_t          addrlen = sizeof(src_addr);
			ret                        = recvfrom(client->fd, client->recv_buff, sizeof(client->recv_buff), 0, (struct sockaddr*)&src_addr, &addrlen);

			if (ret < 0)
			{
					perror("Recvfrom error: ");
					exit_program(client);
			}

			if (verify_response(client, src_addr) == ERROR)
				continue;

			get_info_from_response(client, src_addr, attempt);
			break;
		}
	}
}