#ifndef SIGNATURE_H
#define SIGNATURE_H
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>


class Context;


/**
 * The signature of a template, expressing its name, the number of data and
 * content sections it expects, the number of parameters each section
 * expects, and the name of each parameter. Has the ability to inject
 * definitions of its parameters into a Context given a set of values.
 */
class Signature {
public:

	Signature(const std::string&);
	Signature(const std::string&, const Signature&);
	Signature(const std::string&,
		const std::vector<std::vector<std::string>>&,
		const std::vector<std::vector<std::string>>&);

	void bind(Context&,
		const std::vector<std::vector<double>>&,
		const std::vector<std::vector<std::string>>&) const;

	bool matches(const std::string&,
		const std::vector<std::vector<double>>&,
		const std::vector<std::vector<std::string>>&) const;

	friend bool operator<(const Signature&, const Signature&);

	std::string name;
	std::vector<std::pair<std::vector<std::string>, bool>> data;
	std::vector<std::pair<std::vector<std::string>, bool>> content;
	std::string canonical;

};


#endif
