#include "Vision.h"
#include <iostream>
#include <stdexcept>


/**
 * The beginning of a journey is usually really pleasant, when you get right
 * down to it. Everything is clean and shiny and new, and you haven't yet fought
 * any dragons or dealt in any black magic. But of course nothing can end
 * without beginning, and as lightweight software, our responsibility is to end
 * as fast as humanly (figuratively speaking, of course) possible.
 *
 * So take a deep breath and let's get this party started.
 */
int main(int argc, char* argv[]) try {

	Vision(argc, argv).run();

} catch (const std::runtime_error& exception) {

	std::cerr << exception.what() << '\n';
	return 1;

} catch (const std::logic_error& exception) {

	std::cerr << "Internal error:\n" << exception.what() << '\n';
	return 1;

}
