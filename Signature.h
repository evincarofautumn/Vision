#ifndef SIGNATURE_H
#define SIGNATURE_H
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>


class Context;


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
