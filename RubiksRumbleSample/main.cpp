#include "VitaGefDummies.hpp"
#include "ARApp.h"

unsigned int heapSize = 128*1024*1024;

int main(void)
{
	// initialisation
	Dummy::Platform platform;

	ARApp myApp(platform);
	myApp.Run();

	return 0;
}