#include "../includes/traceroute.h"

int icmp_checksum(unsigned char* buff, int len)
{
		const uint16_t* data = (uint16_t*)buff;
		int             sum  = 0;

		// Le checksum est la somme de tout les mots de 16 bits
		// On additionne tout les bits du paquet mais 2 par 2 pour correspondre a la norme ICMP
		while (len > 1)
		{
				sum += ntohs(*data++);
				len -= 2;
		}

		// Si il reste un octet a la fin
		if (len == 1)
		{
				uint16_t last    = 0;
				*(uint8_t*)&last = *(const uint8_t*)data;
				sum += last;
		}

		// Si la somme depasse 16 bits, on fold
		// sum & 0XFFFF on garde les 16 bits de poids faible
		// sum >> 16 : et on ajouter les bits qui depasses 16 bits
		while (sum >> 16)
		{
				sum = (sum & 0XFFFF) + (sum >> 16);
		}
		// Le checksum ICMP est defini comme le complement a un de la sommes
		// Le complement consiste a inverser tous les bits
		return htons(~sum);
}

int build_echo_request(t_traceroute_client* client, t_icmp_packet* packet)
{
		memset(&packet, 0, sizeof(packet));

		// Type du message ICMP
		// 8 = Echo Request
		// 0 = echo reply
		packet->hdr.type = ICMP_ECHO;

		// Precision du type ICMP
		// Certains ICMP utilisent ce code pour preciser les evenements
		packet->hdr.code = 0;
		// Sert a verifier l'integrite du paquet
		// On met juste 0 temporairement
		packet->hdr.checksum = 0;

		// Identifiant du paquet pour le programme
		// On prend le pid du programme et on garde seulement les 16 bits de poids faible
		// Htons convertit en network byte order pour eviter les problemes
		packet->hdr.un.echo.id = htons(getpid() & 0XFFFF);

		// Numero de sequence du paquet
		// Si je devais envoyer plusieurs ping je pourrai incrementer cette valeur
		packet->hdr.un.echo.sequence = htons(client->seq);
		// On remplit le payload avec un timestamp
		if (PAYLOAD_SIZE < sizeof(struct timeval))
		{
				fprintf(stderr, "Payload size too small for timestamp\n");
				return (ERROR);
		}

		struct timeval tv;
		gettimeofday(&tv, NULL);
		memcpy(packet->payload, &tv, sizeof(tv));

		// client->packet[client->seq].send_time = tv;
		// client->packet[client->seq].received  = false;
		// On remplit le reste du payload avec des zeros
		for (int i = 8 + sizeof(tv); i < 8 + 56; ++i)
		{
				packet->payload[i] = 0;
		}

		// On calcul la taille du paquets
		packet->hdr.checksum = icmp_checksum((unsigned char*)packet, sizeof(struct icmphdr) + PAYLOAD_SIZE);
		return PAYLOAD_SIZE + sizeof(struct icmphdr);
}