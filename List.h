#ifndef LIST_H
#define LIST_H
#include "Value.h"
#include <memory>
#include <vector>


class List : public Value {
public:

	List(int, int);
	List(int, int, const std::vector<std::shared_ptr<const Value>>&);
	virtual ~List();

	void add(std::shared_ptr<const Value>);
	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

	std::vector<std::string> flat_content() const;
	std::vector<double> flat_data() const;

protected:

	virtual List* clone() const;

private:

	std::vector<std::shared_ptr<const Value>> value;

};


#endif
