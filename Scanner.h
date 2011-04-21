#ifndef SCANNER_H
#define SCANNER_H
#include "Token.h"
#include <list>


class Context;


/**
 * Produces a list of Tokens from an input stream with an assumed tab width.
 */
class Scanner {
public:

	Scanner(std::istream&);
	std::list<Token> run(const Context&) const;

private:

	std::istream& stream;

};


#endif
