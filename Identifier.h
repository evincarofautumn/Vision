#ifndef IDENTIFIER_H
#define IDENTIFIER_H
#include "Value.h"
#include <string>


class Identifier : public Value {
public:

	Identifier(int, int, const std::string&);
	virtual ~Identifier();

	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

	std::string value;

protected:

	virtual Identifier* clone() const;

};


#endif
