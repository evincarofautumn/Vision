#ifndef BLOCK_H
#define BLOCK_H
#include "Value.h"
#include <memory>
#include <vector>


/**
 * A block of unevaluated Expressions.
 */
class Block : public Expression {
public:

	Block(int, int);
	Block(int, int, const std::vector<std::shared_ptr<const Expression>>&);
	virtual ~Block();

	void add(std::shared_ptr<const Expression>);
	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

protected:

	virtual Block* clone() const;

private:

	std::vector<std::shared_ptr<const Expression>> value;

};


#endif
