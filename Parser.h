#ifndef PARSER_H
#define PARSER_H
#include <memory>


class Expression;
class Scanner;


/**
 * Parses a list of Tokens into a Block of Expressions.
 */
class Parser {
public:

	Parser(const Scanner&, bool);
	std::shared_ptr<const Expression> run() const;

private:

	const Scanner& scanner;
	const bool indent_mode;

};


#endif
