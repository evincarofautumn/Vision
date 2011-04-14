#include "Content.h"
#include "List.h"
#include <sstream>

#include <iostream>


Content::Content(int line, int column, const std::string& value) :
	Value(line, column), value(value) {}


Content::~Content() {}


std::shared_ptr<const List> Content::evaluate(Context&) const {
	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(self_reference());
	return std::static_pointer_cast<const List>(result);
}


std::string Content::get_content() const {
	return value;
}


double Content::get_data() const {
	std::istringstream stream(value);
	double result;
	if (stream >> result)
		return result;
	return 0;
}


Content* Content::clone() const { return new Content(*this); }
