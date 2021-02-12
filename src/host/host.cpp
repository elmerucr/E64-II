#include <cstdio>

#include "host.hpp"
#include "common.hpp"

E64::host_t::host_t()
{
	printf("[host] E64-II (C)%i by elmerucr - version %i.%i.%i\n",
	       E64_II_YEAR, E64_II_MAJOR_VERSION, E64_II_MINOR_VERSION,
	       E64_II_BUILD);
}

E64::host_t::~host_t()
{
	printf("[host] closing E64-II\n");
}
