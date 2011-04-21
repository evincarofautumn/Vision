#ifndef CONTENT_H
#define CONTENT_H
#include "Value.h"
#include <string>


/**
 * A string Value.
 */
class Content : public Value {
public:

	Content(int, int, const std::string&);
	virtual ~Content();

	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

protected:

	virtual Content* clone() const;

private:

	std::string value;

};


#endif
