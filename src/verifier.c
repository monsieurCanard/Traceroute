#include <netinet/in.h>

#include "../includes/traceroute.h"

extern bool g_exit_program;

static t_data_icmp extract_paquet_icmp(unsigned char* buff)
{
	t_data_icmp data;

	data.ip_header    = (struct iphdr*)buff;
	int ip_header_len = data.ip_header->ihl * 4;
	data.data         = (struct icmphdr*)(buff + ip_header_len);

	return data;
}

static struct icmphdr* get_inner_icmp_info(t_data_icmp icmp)
{
	struct iphdr *original_ip = (struct iphdr*)((unsigned char*)icmp.data + sizeof(struct icmphdr));
	int original_ip_header_len = original_ip->ihl * 4;
	struct icmphdr *inner_icmp = (struct icmphdr*)((unsigned char*)original_ip + original_ip_header_len);
	return inner_icmp;
}



static bool paquet_valid(t_traceroute_client* client, t_data_icmp icmp)
{

	unsigned char icmp_buf[sizeof(struct icmphdr) + PAYLOAD_SIZE] = {0};
	memcpy(icmp_buf, icmp.data, sizeof(struct icmphdr) + PAYLOAD_SIZE);

	uint16_t        original_checksum = icmp.data->checksum;
	uint16_t        recv_seq          = ntohs(icmp.data->un.echo.sequence);
	struct icmphdr* icmp_check        = (struct icmphdr*)icmp_buf;
	icmp_check->checksum              = 0;
	uint16_t recv_checksum =
		icmp_checksum((unsigned char*)icmp_check, sizeof(struct icmphdr) + PAYLOAD_SIZE);

	struct in_addr addr;
	addr.s_addr = icmp.ip_header->saddr;

	uint8_t type = icmp.data->type;
	struct icmphdr *inner_icmp = NULL;
	switch (type)
	{
		case ICMP_ECHOREPLY:
			inner_icmp = icmp.data;
			break;
		case ICMP_HOST_UNREACH:
			inner_icmp = get_inner_icmp_info(icmp);
			break;
		case ICMP_TIME_EXCEEDED:
			inner_icmp = get_inner_icmp_info(icmp);
			break;
		default:
			return false;
	}

	if (ntohs(inner_icmp->un.echo.id) != (getpid() & 0xFFFF))
	{
		printf("From %s : Received packet with invalid identifier\n ", inet_ntoa(addr));
		return false;
	}

	if (recv_checksum != original_checksum)
	{
		printf("From %s: Received packet with invalid checksum\n ", inet_ntoa(addr));
		return false;
	}

	if (recv_seq > client->seq)
	{
		printf("From %s : Received packet with invalid sequence number\n ", inet_ntoa(addr));
		return false;
	}
	return true;
}
/**
 * @note This function exit if the target is reached, otherwise it just verifies the response and returns an error code if the packet is invalid.
 */
int verify_response(t_traceroute_client* client,struct sockaddr_in src_addr)
{
	t_data_icmp icmp = extract_paquet_icmp(client->recv_buff);

	if (!paquet_valid(client, icmp)) {
		return (ERROR);
	}

	if ((src_addr.sin_addr.s_addr == client->sockaddr.sin_addr.s_addr)
		&& (icmp.data->code == ICMP_ECHOREPLY))
	{
		client->target_reached = true;
	}
	return (SUCCESS);
}