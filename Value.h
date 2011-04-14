#ifndef VALUE_H
#define VALUE_H
#include "Expression.h"


class Value : public Expression, public std::enable_shared_from_this<Value> {
public:

	Value(int, int);
	virtual ~Value();

protected:

	std::shared_ptr<const Value> self_reference() const;
	virtual Value* clone() const = 0;

};


#endif
