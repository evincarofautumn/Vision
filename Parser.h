#ifndef PARSER_H
#define PARSER_H
#include <memory>


class Context;
class Expression;
class Scanner;


/**
 * Parses a list of Tokens into a Block of Expressions.
 */
class Parser {
public:

	Parser(const Scanner&);
	std::shared_ptr<const Expression> run(const Context&) const;

private:

	const Scanner& scanner;

};


#endif
