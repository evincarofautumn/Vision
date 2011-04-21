#include "Data.h"
#include "List.h"
#include <iomanip>
#include <limits>
#include <sstream>


Data::Data(int line, int column, double value) : Value(line, column),
	value(value) {}


Data::~Data() {}


/**
 * A numeric Value as a List is, go figure, a List of only that Value.
 */
std::shared_ptr<const List> Data::evaluate(Context&) const {
	std::shared_ptr<List> result(new List(line_number, column_number));
	result->add(self_reference());
	return std::static_pointer_cast<const List>(result);
}


/**
 * Kind of a stringly-typed language, so let's preserve precision.
 */
std::string Data::get_content() const {
	std::ostringstream stream;
	stream << std::setprecision(std::numeric_limits<double>::digits10)
		<< value;
	return stream.str();
}


double Data::get_data() const { return value; }


Data* Data::clone() const { return new Data(*this); }
