#include "Interpreter.h"
#include "Context.h"
#include "Expression.h"
#include "List.h"
#include "Parser.h"
#include <ostream>


Interpreter::Interpreter(const Parser& parser, std::ostream& stream) :
	parser(parser), stream(stream) {}


void Interpreter::run() {

	std::shared_ptr<const Expression> expression = parser.run();
	auto result = expression->evaluate(context);
	stream << context.head_buffer.str() << '\n' << result->get_content();

}
