#include "Block.h"
#include "List.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include <iostream>


Block::Block(int line, int column) : Expression(line, column) {}


Block::Block(int line, int column,
	const std::vector<std::shared_ptr<const Expression>>& value) :
	Expression(line, column), value(value) {}


Block::~Block() {}


void Block::add(std::shared_ptr<const Expression> expression) {
	value.push_back(expression);
}


/**
 * Evaluate each Expression and yield a List of results.
 */
std::shared_ptr<const List> Block::evaluate(Context& context) const {
	std::shared_ptr<List> result(new List(line_number, column_number));
	for (auto i = value.begin(); i != value.end(); ++i)
		result->add((*i)->evaluate(context));
	return std::static_pointer_cast<const List>(result);
}


/**
 * Can't get there from here.
 */
std::string Block::get_content() const {
	throw std::logic_error
		("Attempt to get content from block without context.");
}


/**
 * Seriously, can't get that from this.
 */
double Block::get_data() const {
	throw std::logic_error
		("Attempt to get data from block without context.");
}


Block* Block::clone() const { return new Block(*this); }
