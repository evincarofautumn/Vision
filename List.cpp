#include "List.h"
#include <algorithm>
#include <sstream>

#include <iostream>


List::List(int line, int column) : Value(line, column) {}


List::List(int line, int column,
	const std::vector<std::shared_ptr<const Value>>& value) :
	Value(line, column), value(value) {}


List::~List() {}


void List::add(std::shared_ptr<const Value> element) {
	if (std::shared_ptr<const List> list =
		std::dynamic_pointer_cast<const List>(element)) {
		for (auto i = list->value.begin(); i != list->value.end(); ++i)
			value.push_back(*i);
	} else {
		value.push_back(element);
	}
}


std::shared_ptr<const List> List::evaluate(Context&) const {
	return std::static_pointer_cast<const List>(self_reference());
}


std::string List::get_content() const {
	std::ostringstream stream;
	for (auto i = value.begin(); i != value.end(); ++i)
		stream << (*i)->get_content();
	return stream.str();
}


double List::get_data() const {
	return value.empty() ? 0 : value[0]->get_data();
}


std::vector<std::string> List::flat_content() const {
	std::vector<std::string> result;
	for (auto i = value.begin(); i != value.end(); ++i)
		result.push_back((*i)->get_content());
	return result;
}


std::vector<double> List::flat_data() const {
	std::vector<double> result;
	for (auto i = value.begin(); i != value.end(); ++i)
		result.push_back((*i)->get_data());
	return result;
}


List* List::clone() const { return new List(*this); }
