#ifndef GROUP_H
#define GROUP_H
#include "Value.h"
#include <memory>
#include <vector>


class Group : public Value {
public:

	Group(int, int);
	Group(int, int, const std::vector<std::shared_ptr<const Expression>>&);
	virtual ~Group();

	void add(std::shared_ptr<const Expression>);
	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

protected:

	virtual Group* clone() const;

private:

	std::vector<std::shared_ptr<const Expression>> value;

};


#endif
