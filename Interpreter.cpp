#include "Interpreter.h"
#include "Context.h"
#include "Expression.h"
#include "List.h"
#include "Parser.h"
#include <ostream>


Interpreter::Interpreter(const Parser& parser, std::ostream& stream) :
	parser(parser), stream(stream) {}


/**
 * Run the Parser, evaluate the resulting Expression, and send the result of
 * flattening the resulting results to the stream. Of results.
 */
void Interpreter::run() {

	std::shared_ptr<const Expression> expression = parser.run(context);
	auto result = expression->evaluate(context);
	stream << context.head_buffer.str() << '\n';
 	if (!context.head_mode)
		stream << result->get_content();

}
