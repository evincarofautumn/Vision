#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <iosfwd>
#include "Context.h"


class Parser;


/**
 * Interprets an Expression and sends output to a stream.
 */
class Interpreter {
public:

	Interpreter(const Parser&, std::ostream&);
	void run();

	Context context;

private:

	const Parser& parser;
	std::ostream& stream;

};


#endif
