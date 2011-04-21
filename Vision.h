#ifndef VISION_H
#define VISION_H
#include <map>
#include <string>


class Context;


/**
 * The Vision runtime. Accepts command-line and CGI parameters, sets runtime
 * options, and coordinates the activities of the Scanner, Parser, and
 * Interpreter.
 */
class Vision {
public:

	Vision(int, char**);
	void run() const;

	enum OutputFormat {
		TEXT = 0,
		CPP,
		MINIFIED,
	};

private:

	void parse_options(int, char**);
	void parse_environment();
	void decode_variables(const std::string&);
	void define_input(Context&) const;

	std::string filename;
	OutputFormat output_format;
	bool indent_mode;
	bool pedantic_mode;
	bool silent_mode;
	bool head_mode;
	int tab_size;

	int content_length;
	std::map<std::string, std::string> cgi;
	std::map<std::string, std::string> input;

};


#endif
