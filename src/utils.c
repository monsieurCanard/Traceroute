#include "../includes/traceroute.h"

static double calculate_rtt(struct timeval send_time, struct timeval recv_time)
{
	double rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 + (recv_time.tv_usec - send_time.tv_usec) / 1000.0;
	return rtt;
}

bool msg_is_timeout(struct timeval send_time, struct timeval now)
{
	double rtt = calculate_rtt(send_time, now);
	return rtt >= (TIMEOUT_SEC * 1000.0 + TIMEOUT_USEC / 1000.0);
}