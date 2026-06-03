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

		//TIMEOUT
		if (ret == 0)
		{
			client->hop_packet.rtt[attempt] = -1.0; // Indicate a timeout with a negative RTT
			break;
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

			gettimeofday(&client->now, NULL);
			
			struct sockaddr_in src_addr;
			socklen_t          addrlen = sizeof(src_addr);
			ret                        = recvfrom(
					client->fd, client->recv_buff, sizeof(client->recv_buff), 0, (struct sockaddr*)&src_addr, &addrlen);

			if (ret < 0)
			{
				//TODO : handle recvfrom error (retry or exit)
					perror("Recvfrom error: ");
					continue;
			}

			if (src_addr.sin_addr.s_addr == client->sockaddr.sin_addr.s_addr)
			{
				client->target_reached = true;
				exit_program(client);
			}

			client->hop_packet.rtt[attempt] = (client->now.tv_sec - client->hop_packet.send_time[attempt].tv_sec) * 1000.0 +
																			 (client->now.tv_usec - client->hop_packet.send_time[attempt].tv_usec) / 1000.0;

			int ret = getnameinfo((struct sockaddr*)&src_addr, sizeof(src_addr), client->hop_packet.hosts[attempt], sizeof(client->hop_packet.hosts[attempt]), NULL, 0, 0);
			if (ret != 0)
			{
				snprintf(client->hop_packet.hosts[attempt], sizeof(client->hop_packet.hosts[attempt]), "%s", inet_ntoa(src_addr.sin_addr));
			} else
			{
				snprintf(client->hop_packet.ips[attempt], sizeof(client->hop_packet.ips[attempt]), "%s", inet_ntoa(src_addr.sin_addr));
			}

			break;
		}
	}
}