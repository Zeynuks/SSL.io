#include <cstdlib>
#include <iostream>

#include "Example/Example.h"
#include "Example/Example2.h"

int main()
{
	//flecs realization attempt
	Example2 example;
	example.Start();

	//own realization
	//Example example;
	//example.Start();

	return EXIT_SUCCESS;
}