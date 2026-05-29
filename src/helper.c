#include "../includes/traceroute.h"

void print_helper() {
	printf("Usage: traceroute [OPTIONS] DESTINATION\n");
	printf("\nOptions:\n");
	printf("  -h, --help     Display this help message\n");
	printf("\nNote: Traceroute must be run as root\n");
}