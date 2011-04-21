#include "Compound.h"
#include "Block.h"
#include "Content.h"
#include "Context.h"
#include "Data.h"
#include "Identifier.h"
#include "Interpreter.h"
#include "List.h"
#include "Parser.h"
#include "Scanner.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


/**
 * Compound expressions are where all the fun happens; in order to make the
 * implementation of said fun at all close to fun to maintain, each keyword
 * (generally speaking) gets its own implementation function, which is
 * associated with the builtin name via this map.
 */
decltype(Compound::evaluators) Compound::evaluators {

	std::make_pair("def",       &Compound::evaluate_def),
	std::make_pair("error",     &Compound::evaluate_error),
	std::make_pair("extern",    &Compound::evaluate_extern),
	std::make_pair("file",      &Compound::evaluate_file),
	std::make_pair("header",    &Compound::evaluate_header),
	std::make_pair("if",        &Compound::evaluate_if),
	std::make_pair("local",     &Compound::evaluate_local),
	std::make_pair("namespace", &Compound::evaluate_namespace),
	std::make_pair("use",       &Compound::evaluate_use),
	std::make_pair("using",     &Compound::evaluate_using),
	std::make_pair("warn",      &Compound::evaluate_warn),

	std::make_pair("+",         &Compound::evaluate_math),
	std::make_pair("-",         &Compound::evaluate_math),
	std::make_pair("*",         &Compound::evaluate_math),
	std::make_pair("/",         &Compound::evaluate_math),
	std::make_pair("%",         &Compound::evaluate_math),

	std::make_pair("&",         &Compound::evaluate_math),
	std::make_pair("|",         &Compound::evaluate_math),
	std::make_pair("!",         &Compound::evaluate_math),

	std::make_pair("<",         &Compound::evaluate_math),
	std::make_pair(">=",        &Compound::evaluate_math),
	std::make_pair("=",         &Compound::evaluate_math),
	std::make_pair("<>",        &Compound::evaluate_math),
	std::make_pair(">",         &Compound::evaluate_math),
	std::make_pair("<=",        &Compound::evaluate_math),

};


/**
 * Most mathematical builtins take two operands, but that can vary, or possibly
 * change in the future. As much as I am a fan of YAGNI, it is sometimes better
 * to be safe than sorry.
 */
decltype(Compound::math_arities) Compound::math_arities {

	std::make_pair("+",  2),
	std::make_pair("-",  2),
	std::make_pair("*",  2),
	std::make_pair("/",  2),
	std::make_pair("%",  2),

	std::make_pair("&",  2),
	std::make_pair("|",  2),
	std::make_pair("!",  1),

	std::make_pair("<",  2),
	std::make_pair(">=", 2), 
	std::make_pair("=",  2),
	std::make_pair("<>", 2),
	std::make_pair(">",  2),
	std::make_pair("<=", 2),

};


// Forward declarations:

Compound::MathFunction
	math_add,
	math_subtract,
	math_multiply,
	math_divide,
	math_modulus,
	math_and,
	math_or,
	math_not,
	math_less,
	math_not_less,
	math_equal,
	math_not_equal,
	math_greater,
	math_not_greater;
	// "I love you, Angeline. I've always loved you. Your husband be damned."


/**
 * Math builtins are mapped to their implementations here.
 */
decltype(Compound::math_functions) Compound::math_functions {
	std::make_pair("+",  math_add),
	std::make_pair("-",  math_subtract),
	std::make_pair("*",  math_multiply),
	std::make_pair("/",  math_divide),
	std::make_pair("%",  math_modulus),
	std::make_pair("&",  math_and),
	std::make_pair("|",  math_or),
	std::make_pair("!",  math_not),
	std::make_pair("<",  math_less),
	std::make_pair(">=", math_not_less),
	std::make_pair("=",  math_equal),
	std::make_pair("<>", math_not_equal),
	std::make_pair(">",  math_greater),
	std::make_pair("<=", math_not_greater),
};


// Obvious bits.


Compound::Compound(int line, int column) : Expression(line, column) {}


Compound::~Compound() {}


void Compound::set_determiner(std::shared_ptr<const Expression> expression) {
	determiner = expression;
}


void Compound::set_identifier(const std::string& string) {
	identifier = string;
}


void Compound::add_data() {
	data.push_back(std::vector<std::shared_ptr<const Expression>>());
}


void Compound::add_data(std::shared_ptr<const Expression> expression) {
	if (data.empty()) add_data();
	data.back().push_back(expression);
}


void Compound::add_content() {
	content.push_back(std::vector<std::shared_ptr<const Expression>>());
}


void Compound::add_content(std::shared_ptr<const Expression> expression) {
	if (content.empty()) add_content();
	content.back().push_back(expression);
}


bool Compound::is_keyword(const std::string& name) {
	return evaluators.find(name) != evaluators.end();
}


/**
 * To evaluate a Compound Expression, just look up what sort of Expression it
 * is, and bang, you're done. There are some crufty bits to account for
 * parameter passing and errors, of course.
 */
std::shared_ptr<const List> Compound::evaluate(Context& context) const {

	std::string id;

	if (std::dynamic_pointer_cast<const Identifier>(determiner))
		id = std::static_pointer_cast<const Identifier>(determiner)->value;
	else
		id = determiner->evaluate(context)->get_content();

	try {

		auto evaluator = evaluators.find(id);

		if (evaluator != evaluators.end())
			return (this->*evaluator->second)(id, context);

		std::vector<std::vector<double>> data_parameters;
		std::vector<std::vector<std::string>> content_parameters;

		for (auto i = data.begin(); i != data.end(); ++i) {
			List flattener(line_number, column_number);
			for (auto j = i->begin(); j != i->end(); ++j)
				flattener.add((*j)->evaluate(context));
			data_parameters.push_back(flattener.flat_data());
		}

		for (auto i = content.begin(); i != content.end(); ++i) {
			List flattener(line_number, column_number);
			for (auto j = i->begin(); j != i->end(); ++j)
				flattener.add((*j)->evaluate(context));
			content_parameters.push_back(flattener.flat_content());
		}

		return context.evaluate(id, data_parameters, content_parameters);

	} catch (const std::runtime_error& exception) {

		std::ostringstream message;
		message << "In ";
		if (is_keyword(id))
			message << id;
		else
			message << "template";
		message << " expression at line " << line_number << ", column "
			<< column_number << ":\n" << exception.what();
		throw std::runtime_error(message.str());

	}

}


/**
 * Evaluate a "def" expression, defining a new template in the current Context.
 */
std::shared_ptr<const List> Compound::evaluate_def(const std::string& id,
	Context& context) const {

	if (is_keyword(identifier)) {
		std::ostringstream message;
		message << "Attempt to define template with reserved name \""
			<< identifier << "\".";
		throw std::runtime_error(message.str());
	}

	std::vector<std::vector<std::string>> data_parameters;
	std::vector<std::vector<std::string>> content_parameters;

	// All things must have a name...
	for (auto i = data.begin(); i != data.end(); ++i) {
		data_parameters.push_back(std::vector<std::string>());
		for (auto j = i->begin(); j != i->end(); ++j) {
			if (!std::dynamic_pointer_cast<const Identifier>(*j)) {
				std::ostringstream message;
				message << "Attempt to define template \""
					<< identifier << "\" with invalid data signature.";
				throw std::runtime_error(message.str());
			}
			data_parameters.back().push_back
				(std::static_pointer_cast<const Identifier>(*j)->value);
		}
	}

	// ...and we cannot understand names that are too complex.
	if (content.size() > 1) {
		auto content_pre_end = content.begin();
		std::advance(content_pre_end, content.size() - 1);
		for (auto i = content.begin(); i != content_pre_end; ++i) {
			content_parameters.push_back(std::vector<std::string>());
			for (auto j = i->begin(); j != i->end(); ++j) {
				if (!std::dynamic_pointer_cast<const Identifier>(*j)) {
					std::ostringstream message;
					message << "Attempt to define template \""
						<< identifier << "\" with invalid content signature.";
					throw std::runtime_error(message.str());
				}
				content_parameters.back().push_back
					(std::static_pointer_cast<const Identifier>(*j)->value);
			}
		}
	}

	Signature signature(identifier, data_parameters, content_parameters);
	context.define(signature, std::shared_ptr<const Expression>
		(new Block(line_number, column_number, content.back())));

	return std::shared_ptr<const List>(new List(line_number, column_number));

}


/**
 * Die with a user-defined error message.
 */
std::shared_ptr<const List> Compound::evaluate_error
	(const std::string& id, Context& context) const {

	if (data.size() != 0 || content.size() != 1)
		throw std::runtime_error("Invalid use of \"error\".");

	std::string message = Block(line_number, column_number,
		content[0]).evaluate(context)->get_content();

	throw std::runtime_error("Error: " + message);

}


/**
 * Do some really filthy stuff with a strange program.
 */
std::shared_ptr<const List> Compound::evaluate_extern
	(const std::string& id, Context& context) const {

	const bool closed_form = !identifier.empty();
	const bool has_input = closed_form ?
		content.size() == 1 : content.size() == 2;

	if (data.size() != 0 || (closed_form && content.size() > 1) ||
		content.size() > 2)
		throw std::runtime_error("Invalid use of \"extern\".");

	std::string command = closed_form ? identifier :  Block(line_number,
		column_number, content[0]).evaluate(context)->get_content();

	std::string input = has_input ? Block(line_number, column_number,
		closed_form ? content[0] : content[1]).evaluate
		(context)->get_content() : "";

	// By the way, it's very important that you don't perform any evaluation
	// while you're writing to the (admittedly awkward, but hey) temporary file.
	// If you can't see why, consider "extern[x]{y extern[x]{z}}".
	// Single-threaded applications for the win.
	if (has_input) {

		command += " < .vision.temp";
		std::ofstream file(".vision.temp");
		if (!file.is_open())
			return std::shared_ptr<const List>
				(new List(line_number, column_number));
		file << input;
		file.close();

	}

	std::ostringstream output;
	FILE* pipe = popen(command.c_str(), "r");
	int c;
	while ((c = std::getc(pipe)) != EOF) output.put(c);
	pclose(pipe);

	if (has_input)
		std::remove(".vision.temp");

	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(std::shared_ptr<const Value>(new Content
		(line_number, column_number, output.str())));
	return std::static_pointer_cast<const List>(result);

}


/**
 * Expand the contents of a file as content.
 */
std::shared_ptr<const List> Compound::evaluate_file
	(const std::string& id, Context& context) const {

	if (data.size() != 0 || content.size() != 1)
		throw std::runtime_error("Invalid use of \"file\".");

	std::string contents;
	std::ifstream file(Block(line_number, column_number,
		content[0]).evaluate(context)->get_content().c_str());

	if (!file.is_open())
		return std::shared_ptr<const List>
			(new List(line_number, column_number));

	std::copy(std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>(),
		std::back_inserter(contents));

	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(std::shared_ptr<const Value>(new Content
		(line_number, column_number, contents)));
	return std::static_pointer_cast<const List>(result);

}


/**
 * Send some content to the header buffer.
 */
std::shared_ptr<const List> Compound::evaluate_header
	(const std::string& id, Context& context) const {

	if (data.size() != 0)
		throw std::runtime_error("Invalid use of \"header\".");

	for (auto i = content.begin(); i != content.end(); ++i) {
		for (auto j = i->begin(); j != i->end(); ++j)
			context.head_buffer << (*j)->evaluate(context)->get_content();
		context.head_buffer << '\n';
	}

	return std::shared_ptr<const List>(new List(line_number, column_number));

}


/**
 * Conditionally evaluate some Expressions.
 */
std::shared_ptr<const List> Compound::evaluate_if
	(const std::string& id, Context& context) const {

	if (data.size() != 1 || content.size() != 1)
		throw std::runtime_error("Invalid use of \"if\".");

	double condition = Block(line_number, column_number,
		data[0]).evaluate(context)->get_data();

	if (condition != 0.0)
		return Block(line_number, column_number, content[0]).evaluate(context);

	return std::shared_ptr<const List>(new List(line_number, column_number));

}


/**
 * Evaluate in a new local scope.
 */
std::shared_ptr<const List> Compound::evaluate_local
	(const std::string& id, Context& context) const {

	if (content.size() != 1)
		throw std::runtime_error("Invalid use of \"local\".");

	context.enter_scope();
	auto result = Block
		(line_number, column_number, content[0]).evaluate(context);
	context.exit_scope();
	return result;

}


/**
 * Evaluate a math expression.
 */
std::shared_ptr<const List> Compound::evaluate_math
	(const std::string& id, Context& context) const {

	const int arity = math_arities.find(id)->second;

	if (data.size() != arity || !content.empty()) {
		std::ostringstream message;
		message << "Invalid use of \"" << id << "\".";
		throw std::runtime_error(message.str());
	}

	std::vector<double> operands;

	for (auto i = data.begin(); i != data.end(); ++i)
		operands.push_back(Block(line_number, column_number, *i).evaluate
			(context)->get_data());

	const auto function = math_functions.find(id)->second;
	const double value = function(operands);

	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(std::shared_ptr<const Value>(new Data
		(line_number, column_number, value)));

	return std::static_pointer_cast<const List>(result);

}


// More obvious stuff.


double math_add(const std::vector<double>& operands) {
	return operands[0] + operands[1];
}


double math_subtract(const std::vector<double>& operands) {
	return operands[0] - operands[1];
}


double math_multiply(const std::vector<double>& operands) {
	return operands[0] * operands[1];
}


double math_divide(const std::vector<double>& operands) {
	if (operands[1] == 0)
		throw std::runtime_error("Division by zero.");
	return operands[0] / operands[1];
}


double math_modulus(const std::vector<double>& operands) {
	if (operands[1] == 0)
		throw std::runtime_error("Division by zero.");
	return std::fmod(operands[0], operands[1]);
}


double math_and(const std::vector<double>& operands) {
	return operands[0] && operands[1];
}


double math_or(const std::vector<double>& operands) {
	return operands[0] || operands[1];
}


double math_not(const std::vector<double>& operands) {
	return !operands[0];
}


double math_less(const std::vector<double>& operands) {
	return operands[0] < operands[1];
}


double math_not_less(const std::vector<double>& operands) {
	return operands[0] >= operands[1];
}


double math_equal(const std::vector<double>& operands) {
	return operands[0] == operands[1];
}


double math_not_equal(const std::vector<double>& operands) {
	return operands[0] != operands[1];
}


double math_greater(const std::vector<double>& operands) {
	return operands[0] > operands[1];
}


double math_not_greater(const std::vector<double>& operands) {
	return operands[0] <= operands[1];
}


/**
 * Evaluate in a named local scope.
 */
std::shared_ptr<const List> Compound::evaluate_namespace
	(const std::string& id, Context& context) const {

	context.enter_scope(identifier);
	auto result = Block
		(line_number, column_number, content[0]).evaluate(context);
	context.exit_scope();
	return result;

}


/**
 * Import a module.
 */
std::shared_ptr<const List> Compound::evaluate_use
	(const std::string& id, Context& context) const {

	std::string name;
	if (content.size() == 1 && identifier.empty() && data.empty())
		name = Block(line_number, column_number, content[0]).evaluate
			(context)->get_content();
	else if (!identifier.empty() && content.empty() && data.empty())
		name = identifier;
	else
		throw std::runtime_error("Invalid use of \"use\".");

	std::ifstream file(name.c_str());

	if (!file.is_open()) {
		std::ostringstream message;
		message << "Unable to find library \"" << name << "\".";
		throw std::runtime_error(message.str());
	}

	std::ostringstream output;
	const Scanner scanner(file);
	const Parser parser(scanner);
	Interpreter interpreter(parser, output);
	interpreter.run();
	context.inject(interpreter.context);
	context.head_buffer << interpreter.context.head_buffer.str();

	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(std::shared_ptr<const Value>(new Content
		(line_number, column_number, output.str())));
	return std::static_pointer_cast<const List>(result);

}


/**
 * Import a namespace prefix into the current scope.
 */
std::shared_ptr<const List> Compound::evaluate_using
	(const std::string& id, Context& context) const {

	context.use(identifier);
	return std::shared_ptr<const List>(new List(line_number, column_number));

}


/**
 * Complain with a user-defined warning message, or die if in pedantic mode.
 */
std::shared_ptr<const List> Compound::evaluate_warn
	(const std::string& id, Context& context) const {

	if (data.size() != 0 || content.size() != 1)
		throw std::runtime_error("Invalid use of \"error\".");

	std::string message = Block(line_number, column_number,
		content[0]).evaluate(context)->get_content();

	if (!context.silent_mode) {
		if (context.pedantic_mode)
			throw std::runtime_error(message);
		std::cerr << "Warning: " << message << '\n';
	}

	return std::shared_ptr<const List>(new List(line_number, column_number));

}


/**
 * You really can't evaluate some things without a context.
 */
std::string Compound::get_content() const {
	throw std::logic_error
		("Attempt to get content from compound expression without context.");
}


/**
 * No really, I don't know why I have to keep harping on this.
 */
double Compound::get_data() const {
	throw std::logic_error
		("Attempt to get data from compound expression without context.");
}
