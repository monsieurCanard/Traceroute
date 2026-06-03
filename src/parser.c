#include "../includes/traceroute.h"

static void print_helper() {
	printf("Usage: traceroute [OPTIONS] DESTINATION\n");
	printf("\nOptions:\n");
	printf("  -h, --help     Display this help message\n");
	printf("\nNote: Traceroute must be run as root\n");
}

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
	// Verify if the program is run as root (place in the parser because it's one of the first thing we check in the program)
	if (getuid() != 0)
	{
		fprintf(stderr, "traceroute: must be run as root\n");
		return (ERROR);
	}

	return (SUCCESS);
}