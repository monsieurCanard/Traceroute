#include "../includes/traceroute.h"

extern bool g_exit_program;

void main_loop(t_traceroute_client* client)
{
		printf("Main loop started\n");
		unsigned char  recv_buff[1024];

		struct timeval send_time, now;
		int current_hop = 0;

		struct timeval timeout;
		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;

		
		printf("Starting while loop\n");
		while (!g_exit_program && current_hop + 1 < MAX_HOPS)
		{
			current_hop += 1;
			printf("%d ", current_hop);
			
			int ret = setsockopt(client->fd, IPPROTO_IP, IP_TTL, &current_hop, sizeof(current_hop));
			if (ret < 0)
			{
				perror("setsockopt error: ");
				exit_program(client);
			}
			
			printf("Sending message for hop %d\n", current_hop);
			for(int i = 0; i < 3; i++)
			{
				
				gettimeofday(&send_time, NULL);
				if (send_message(client, client->sockaddr) == EXIT_FAILURE)
				{
					break;
				}
				
				printf("Message sent for hop %d\n", current_hop);
				
				while(!g_exit_program)
				{
					FD_ZERO(&client->read_fds);
					FD_SET(client->fd, &client->read_fds);

					printf("Waiting for response...\n");
					int ret = select(client->fd + 1, &client->read_fds, NULL, NULL, &timeout);
					if (ret < 0)
					{
							if (errno != EINTR)
									client->status = EXIT_FAILURE;

							exit_program(client);
					}
					if (ret == 0)
					{
						printf("* ");
						break;
					}
					
					if (ret == 1)
					{
							if (!FD_ISSET(client->fd, &client->read_fds))
									continue;
							
							gettimeofday(&send_time, NULL);

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
							printf("Received response from %s\n", inet_ntoa(src_addr.sin_addr));
							// double new_rtt = verify_response_and_print(client, recv_buff, recv_time);
					}
				
				}
			}

			
		}
		exit_program(client);
}