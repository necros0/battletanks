#include <vld.h>

#include "Game.h"


int main (int argc, const char* argv[])
{
	//randomize timer
	srand (time(NULL));

	// NOTE: using static libraries to avoid a bug in SFML:
	// https://github.com/LaurentGomila/SFML/issues/30
	
	Game::instance().start();
	Game::instance().run();
	Game::instance().end();

	return 0;
}