#include "Vision.h"
#include <iostream>
#include <stdexcept>


int main(int argc, char* argv[]) try {

	Vision(argc, argv).run();

} catch (const std::runtime_error& exception) {

	std::cerr << exception.what() << '\n';
	return 1;

} catch (const std::logic_error& exception) {

	std::cerr << "Internal error:\n" << exception.what() << '\n';
	return 1;

}
