#include "../includes/traceroute.h"

extern bool g_exit_program;

void main_loop_icmp(t_traceroute_client* client)
{
		unsigned char  recv_buff[sizeof(struct iphdr) + sizeof(struct icmphdr) + PAYLOAD_SIZE];
		struct timeval send_time, now;
		int current_hop = 0;

		struct timeval timeout;
		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;

		gettimeofday(&send_time, NULL);
		
		while (!g_exit_program && current_hop < MAX_HOPS)
		{
			current_hop++;
			printf("%i ", current_hop);
			
			FD_ZERO(&client->read_fds);
			FD_SET(client->fd, &client->read_fds);

			gettimeofday(&now, NULL);
			int ret = setsockopt(client->fd, IPPROTO_IP, IP_TTL, &current_hop, sizeof(current_hop));
			if (ret < 0)
			{
				perror("setsockopt error: ");
				exit_program(client);
			}

			for(int i = 0; i < 3; i++)
			{

				send_message(client, client->sockaddr);

				int ret = select(client->fd + 1, &client->read_fds, NULL, NULL, &timeout);
				if (ret == 1)
				{
						if (!FD_ISSET(client->fd, &client->read_fds))
								continue;

						struct sockaddr_in src_addr;
						socklen_t          addrlen = sizeof(src_addr);
						ret                        = recvfrom(
								client->fd, recv_buff, sizeof(recv_buff), 0, (struct sockaddr*)&src_addr, &addrlen);
						if (ret < 0)
						{
								perror("Recvfrom error: ");
								continue;
						}

						gettimeofday(&now, NULL);
						double new_rtt = verify_response_and_print(client, recv_buff, recv_time);
				}
				if (ret < 0)
				{
						if (errno != EINTR)
								client->status = EXIT_FAILURE;

						exit_program(client);
				}

			}

				if (time_checker(client, &start_time, &now, &send_time) == RESEND)
				{
						send_message(client, client->sockaddr);
						gettimeofday(&send_time, NULL);
				}

			
		}
		exit_program(client);
}