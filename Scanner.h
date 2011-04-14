#ifndef SCANNER_H
#define SCANNER_H
#include "Token.h"
#include <list>


class Scanner {
public:

	Scanner(std::istream&, int);
	std::list<Token> run() const;

private:

	std::istream& stream;
	const int tab_size;

};


#endif
