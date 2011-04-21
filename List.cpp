#include "List.h"
#include <algorithm>
#include <sstream>


List::List(int line, int column) : Value(line, column) {}


List::List(int line, int column,
	const std::vector<std::shared_ptr<const Value>>& value) :
	Value(line, column), value(value) {}


List::~List() {}


/**
 * Add to the List. I know, I know, I claim to be in the immutability camp, but
 * honestly, trade-offs have got to be made somewhere.
 */
void List::add(std::shared_ptr<const Value> element) {
	if (std::shared_ptr<const List> list =
		std::dynamic_pointer_cast<const List>(element)) {
		for (auto i = list->value.begin(); i != list->value.end(); ++i)
			value.push_back(*i);
	} else {
		value.push_back(element);
	}
}


/**
 * A List is a List is a List.
 */
std::shared_ptr<const List> List::evaluate(Context&) const {
	return std::static_pointer_cast<const List>(self_reference());
}


/**
 * Join all of the content from all of the contents.
 */
std::string List::get_content() const {
	std::ostringstream stream;
	for (auto i = value.begin(); i != value.end(); ++i)
		stream << (*i)->get_content();
	return stream.str();
}


/**
 * Grab the first datum. I didn't really know what else to do here.
 */
double List::get_data() const {
	return value.empty() ? 0 : value[0]->get_data();
}


/**
 * Flatten each element into content, but don't flatten the whole container.
 */
std::vector<std::string> List::flat_content() const {
	std::vector<std::string> result;
	for (auto i = value.begin(); i != value.end(); ++i)
		result.push_back((*i)->get_content());
	return result;
}


/**
 * Flatten each element into data, but again, don't flatten the List.
 */
std::vector<double> List::flat_data() const {
	std::vector<double> result;
	for (auto i = value.begin(); i != value.end(); ++i)
		result.push_back((*i)->get_data());
	return result;
}


List* List::clone() const { return new List(*this); }
