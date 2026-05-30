#include "../includes/traceroute.h"

int parse_args(int ac, char** av)
{
	struct option long_options[] = {{"help", no_argument, 0, '?'}};

	int opt;
	opterr = 0;
	while ((opt = getopt_long(ac, av, "?", long_options, NULL)) != ERROR)
	{
		switch (opt)
		{
		case '?':
			print_helper();
			return (ERROR);
		default:
			fprintf(stderr, "Traceroute: unknown option '%c'\n", opt);
			return (ERROR);
		}
	}

	if (optind >= ac)
	{
		fprintf(stderr, "Traceroute: missing destination address\n");
		return (ERROR);
	}
	return (SUCCESS);
}