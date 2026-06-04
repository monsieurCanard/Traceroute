
#include "../includes/traceroute.h"

bool g_exit_program = false;

void set_exit_program(int sig)
{
	(void)sig;
	g_exit_program = true;
}

int main(int ac, char** av)
{
	t_traceroute_client client = {0};

	if (parse_args(ac, av) == ERROR)
		return (EXIT_FAILURE);

	signal(SIGINT, set_exit_program);

	int ret = create_client(&client, av[optind]);
	if (ret == ERROR)
	{
		client.status = EXIT_FAILURE;
		exit_program(&client);
	}
	
	print_start(client);
	main_loop(&client);
	
	return EXIT_SUCCESS;
}