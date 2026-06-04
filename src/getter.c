#include "../includes/traceroute.h"

void get_info_from_response(t_traceroute_client* client, struct sockaddr_in src_addr, int attempt)
{
	double *rtt_ptr = &client->hop_packet[client->current_hop].rtt[attempt];

	*rtt_ptr = (client->now.tv_sec - client->hop_packet[client->current_hop].send_time[attempt].tv_sec) * 1000.0 + (client->now.tv_usec - client->hop_packet[client->current_hop].send_time[attempt].tv_usec) / 1000.0;

	if (*rtt_ptr > TIMEOUT_SEC * 1000.0 + TIMEOUT_USEC / 1000.0)
	{
		*rtt_ptr = -1.0;
		return;
	}

	client->hop_packet[client->current_hop].src_addr[attempt] = src_addr;

	// char *host_ptr = client->hop_packet[client->current_hop].hosts[attempt];
	// char *ip_ptr   = client->hop_packet[client->current_hop].ips[attempt];

	// int ret = getnameinfo((struct sockaddr*)&src_addr, sizeof(src_addr), host_ptr, sizeof(client->hop_packet[client->current_hop].hosts[attempt]), NULL, 0, 0);
	// if (ret != 0)
	// {
	// 	snprintf(host_ptr, sizeof(client->hop_packet[client->current_hop].hosts[attempt]), "%s", inet_ntoa(src_addr.sin_addr));
	// } else {
	// 	snprintf(ip_ptr, sizeof(client->hop_packet[client->current_hop].ips[attempt]), "%s", inet_ntoa(src_addr.sin_addr));
	// }
}