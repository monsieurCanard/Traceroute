#include "../includes/traceroute.h"

static int resolve_host(t_traceroute_client* client, char* address)
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_RAW;

    int status = getaddrinfo(address, NULL, &hints, &res);
    if (status != SUCCESS)
    {
        fprintf(stderr, "Could not resolve hostname %s: %s\n", address, gai_strerror(status));
        return (ERROR);
    }

    memcpy(&client->sockaddr, res->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(res);

    // Convertir l'adresse IP en chaîne de caractères pour l'affichage
    client->ip = inet_ntoa(client->sockaddr.sin_addr);
    return (SUCCESS);
}

static int create_socket(t_traceroute_client* client)
{
    client->fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (client->fd < 0)
    {
        perror("Creation Socket");
        return (ERROR);
    }

    // if (client->args.all_args & OPT_TTL)
    // {
    //     if (setsockopt(
    //             client->fd, IPPROTO_IP, IP_TTL, &client->args.ttl, sizeof(client->args.ttl)) < 0)
    //     {
    //         perror("Setsockopt TTL: ");
    //         close(client->fd);
    //         return (ERROR);
    //     }
    // }

    return (SUCCESS);
}

int create_client(t_traceroute_client* client, char* address)
{
    if (resolve_host(client, address) == ERROR)
        return (ERROR);

    if (create_socket(client) == ERROR)
        return (ERROR);

    client->name = address;
    return (SUCCESS);
}