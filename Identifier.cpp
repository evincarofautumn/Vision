#include "Identifier.h"
#include "Context.h"
#include <stdexcept>

#include <iostream>


Identifier::Identifier(int line, int column, const std::string& value) :
	Value(line, column), value(value) {}


Identifier::~Identifier() {}


/**
 * A name derives its meaning from a Context.
 */
std::shared_ptr<const List> Identifier::evaluate(Context& context) const {
	return context.evaluate(value);
}


/**
 * So you can't get a value from it without such a Context.
 */
std::string Identifier::get_content() const {
	throw std::logic_error("Attempt to evaluate identifier without context.");
}


/**
 * No, really. You can't get a value of any type from a name without a Context.
 */
double Identifier::get_data() const {
	throw std::logic_error("Attempt to evaluate identifier without context.");
}


Identifier* Identifier::clone() const { return new Identifier(*this); }
