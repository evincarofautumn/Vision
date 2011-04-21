#ifndef SCANNER_H
#define SCANNER_H
#include "Token.h"
#include <list>


/**
 * Produces a list of Tokens from an input stream with an assumed tab width.
 */
class Scanner {
public:

	Scanner(std::istream&, int);
	std::list<Token> run() const;

private:

	std::istream& stream;
	const int tab_size;

};


#endif
