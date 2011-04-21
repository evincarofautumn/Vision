#ifndef DATA_H
#define DATA_H
#include "Value.h"


class Context;


/**
 * A numeric Value.
 */
class Data : public Value {
public:

	Data(int, int, double);
	virtual ~Data();

	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

protected:

	virtual Data* clone() const;

private:

	double value;

};


#endif
