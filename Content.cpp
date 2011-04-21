#include "Content.h"
#include "List.h"
#include <sstream>

#include <iostream>


Content::Content(int line, int column, const std::string& value) :
	Value(line, column), value(value) {}


Content::~Content() {}


/**
 * A string Value as a List is just a List of just that Value.
 */
std::shared_ptr<const List> Content::evaluate(Context&) const {
	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(self_reference());
	return std::static_pointer_cast<const List>(result);
}


std::string Content::get_content() const { return value; }


/**
 * Strictly speaking, because of this it is possible to use number formats that
 * Vision doesn't directly support, such as scientific notation, simply by
 * quoting them and relying on runtime conversion. Of course, the conversion
 * can fail, and it does so silently, making that not the best idea ever.
 */
double Content::get_data() const {
	std::istringstream stream(value);
	double result;
	if (stream >> result)
		return result;
	return 0;
}


Content* Content::clone() const { return new Content(*this); }
