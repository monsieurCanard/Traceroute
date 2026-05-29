#include "../includes/traceroute.h"

void exit_program(t_traceroute_client* client)
{
    if (client->fd > 0)
    {

        // double success_rate =
        //     msg_transmitted == 0
        //         ? 0.0
        //         : ((msg_transmitted - client->counter.received) / (double)msg_transmitted) * 100.0;

        // print_traceroute_final_stats(client, success_rate);
        close(client->fd);
    }

    exit(client->status);
}