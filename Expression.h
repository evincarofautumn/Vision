#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <memory>
#include <string>


class Context;
class List;
class Value;


class Expression {
public:

	Expression(int line, int column) : line_number(line),
		column_number(column) {}

	virtual ~Expression() {}

	virtual std::shared_ptr<const List> evaluate(Context&) const = 0;
	virtual std::string get_content() const = 0;
	virtual double get_data() const = 0;

	const int line_number;
	const int column_number;

};


#endif
