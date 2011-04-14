#include "Group.h"
#include "Content.h"
#include "List.h"
#include <sstream>
#include <stdexcept>


Group::Group(int line, int column) : Value(line, column) {}


Group::Group(int line, int column,
	const std::vector<std::shared_ptr<const Expression>>& value) :
	Value(line, column), value(value) {}


Group::~Group() {}


void Group::add(std::shared_ptr<const Expression> expression) {
	value.push_back(expression);
}


std::shared_ptr<const List> Group::evaluate(Context& context) const {

	std::ostringstream result;
	for (auto i = value.begin(); i != value.end(); ++i)
		result << (*i)->evaluate(context)->get_content();

	std::shared_ptr<const Value> content(new Content
		(line_number, column_number, result.str()));
	std::vector<std::shared_ptr<const Value>> vector(1, content);
	std::shared_ptr<const List> list(new List
		(line_number, column_number, vector));

	return list;

}


std::string Group::get_content() const {
	throw std::logic_error
		("Attempt to get content from group without context.");
}


double Group::get_data() const {
	throw std::logic_error
		("Attempt to get data from group without context.");
}


Group* Group::clone() const { return new Group(*this); }
