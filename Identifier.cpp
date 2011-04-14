#include "Identifier.h"
#include "Context.h"
#include <stdexcept>

#include <iostream>


Identifier::Identifier(int line, int column, const std::string& value) :
	Value(line, column), value(value) {}


Identifier::~Identifier() {}


std::shared_ptr<const List> Identifier::evaluate(Context& context) const {
	return context.evaluate(value);
}


std::string Identifier::get_content() const {
	throw std::logic_error("Attempt to evaluate identifier without context.");
}


double Identifier::get_data() const {
	throw std::logic_error("Attempt to evaluate identifier without context.");
}


Identifier* Identifier::clone() const { return new Identifier(*this); }
