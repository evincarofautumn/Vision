#include "Vision.h"
#include "Content.h"
#include "Context.h"
#include "Data.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Scanner.h"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


/**
 * Return the string value of an environment variable.
 */
std::string environment_string(const char* name) {
	const char* result = std::getenv(name);
	return result ? result : "";
}


/**
 * Return the numeric value of an environment variable.
 */
int environment_number(const char* name) {
	std::istringstream stream(environment_string(name));
	int result;
	if (!(stream >> result)) return 0;
	return result;
}


/**
 * Initialise the runtime, setting default options, and producing a sane help
 * message if parsing the command line or CGI environment fails.
 */
Vision::Vision(int argc, char** argv) try : output_format(TEXT),
	indent_mode(false), pedantic_mode(false), silent_mode(false),
	head_mode(false), tab_size(4) {

	parse_options(argc, argv);
	parse_environment();

} catch (const std::runtime_error& exception) {

	std::ostringstream message;
	message << "Invalid command line:\n" << exception.what()
		<< "\nUsage: vision [-h] [-i] [-o FORMAT] [-p] [-s] [-t SIZE] "
		"(FILENAME | -)";
	throw std::runtime_error(message.str());

}


/**
 * Perform basic parsing of command-line options; die if confused.
 */
void Vision::parse_options(int argc, char** argv) {

	--argc;
	++argv;
	std::list<std::string> args(argv, argv + argc);

	std::list<std::string>::iterator option;

	// -h
	if ((option = std::find(args.begin(), args.end(), "-h")) != args.end()) {
		head_mode = true;
		args.erase(option);
	}

	// -i
	if ((option = std::find(args.begin(), args.end(), "-i")) != args.end()) {
		indent_mode = true;
		args.erase(option);
	}

	// -o FORMAT
	if ((option = std::find(args.begin(), args.end(), "-o")) != args.end()) {
		auto value = option;
		++value;
		if (value == args.end())
			throw std::runtime_error("Expected output format after -o.");
		if (*value == "text") {
			output_format = TEXT;
		} else if (*value == "c++") {
			output_format = CPP;
		} else if (*value == "min") {
			output_format = MINIFIED;
		} else {
			std::ostringstream message;
			message << "Invalid output format \"" << *value << "\".";
			throw std::runtime_error(message.str());
		}
		args.erase(option);
		args.erase(value);
	}

	// -p
	if ((option = std::find(args.begin(), args.end(), "-p")) != args.end()) {
		pedantic_mode = true;
		args.erase(option);
	}

	// -s
	if ((option = std::find(args.begin(), args.end(), "-s")) != args.end()) {
		silent_mode = true;
		pedantic_mode = false;
		args.erase(option);
	}

	// -t SIZE
	if ((option = std::find(args.begin(), args.end(), "-t")) != args.end()) {
		auto value = option;
		++value;
		if (value == args.end())
			throw std::runtime_error("Expected size after -t option.");
		std::istringstream stream(*value);
		if (!(stream >> tab_size)) {
			std::ostringstream message;
			message << "Invalid tab size \"" << *value << "\".";
			throw std::runtime_error(message.str());
		}
		args.erase(option);
		args.erase(value);
	}

	if (args.size() != 1)
		throw std::runtime_error("Expected filename or \"-\".");

	// FILENAME | -
	filename = args.front();

}


/**
 * Parse CGI environment variables and CGI input over GET and POST. Handles
 * requests only in the usual application/x-www-form-urlencoded format, and
 * should probably handle multipart/form-data in the future.
 */
void Vision::parse_environment() {

	const char* variables[] = {
		"AUTH_TYPE",
		"CONTENT_TYPE",
		"DOCUMENT_ROOT",
		"GATEWAY_INTERFACE",
		"HTTP_REFERER",
		"HTTP_USER_AGENT",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"QUERY_STRING",
		"REMOTE_ADDR",
		"REMOTE_HOST",
		"REMOTE_IDENT",
		"REMOTE_USER",
		"REQUEST_METHOD",
		"SCRIPT_NAME",
		"SERVER_ADMIN",
		"SERVER_NAME",
		"SERVER_PORT",
		"SERVER_PROTOCOL",
		"SERVER_SOFTWARE",
		0
	};

	for (auto i = variables; *i; ++i)
		cgi[*i] = environment_string(*i);

	const auto& request_method = cgi["REQUEST_METHOD"];

	if (request_method == "GET") {

		decode_variables(cgi["QUERY_STRING"]);

	} else if (request_method == "POST") {

		std::string content;
		content.resize(content_length);
		std::cin.read(&content[0], content_length);
		// TODO: magic for multipart/form-data?
		decode_variables(content);

	} else if (request_method == "HEAD") {

		head_mode = true;

	}

}


/**
 * Get the hexadecimal value of a character.
 */
uint8_t hex_value(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 0xa;
	if (c >= 'A' && c <= 'F') return c - 'A' + 0xa;
	return 0;
}


/**
 * Decode a URL-encoded string.
 */
std::string url_decode(const std::string& encoded) {

	std::ostringstream result;

	for (std::string::size_type i = 0; i < encoded.size(); ++i) {

		if (encoded[i] == '%') {

			const uint8_t upper(hex_value(encoded[i + 1]));
			const uint8_t lower(hex_value(encoded[i + 2]));
			result << char(((upper << 4) | lower) & 0xff);
			i += 2;

		} else if (encoded[i] == '+') {

			result << ' ';

		} else {

			result << encoded[i];

		}

	}

	return result.str();

}


/**
 * Parse an application/x-www-form-urlencoded string into a map.
 */
void Vision::decode_variables(const std::string& raw) {

	std::istringstream query(raw);
	std::string name;
	std::string value;

	while (true) {

		if (!std::getline(query, name, '=')) break;
		input[url_decode(name)] = std::getline(query, value, '&') ?
			url_decode(value) : "";

	}

}


/**
 * Inject CGI and request variables into the Context of an Interpreter.
 */
void Vision::define_input(Context& context) const {

	auto request_method = cgi.find("REQUEST_METHOD");
	context.enter_scope(request_method->second);
	for (auto i = input.begin(); i != input.end(); ++i)
		context.define(Signature(i->first), std::shared_ptr<const Expression>
			(new Content(0, 0, i->second)));
	context.exit_scope();

	context.enter_scope("CGI");
	for (auto i = cgi.begin(); i != cgi.end(); ++i)
		context.define(Signature(i->first), std::shared_ptr<const Expression>
			(new Content(0, 0, i->second)));
	context.define(Signature("CONTENT_LENGTH"), std::shared_ptr<const Expression>
		(new Data(0, 0, content_length)));
	context.exit_scope();

}


/**
 * Set the runtime going. If anything breaks, the generated error message is
 * handily prefixed with the filename.
 */
void Vision::run() const try {

	if (filename == "-") {

		const Scanner scanner(std::cin, tab_size);
		const Parser parser(scanner, indent_mode);
		Interpreter interpreter(parser, std::cout);
		interpreter.context.head_mode = head_mode;
		interpreter.context.silent_mode = silent_mode;
		interpreter.context.pedantic_mode = pedantic_mode;
		define_input(interpreter.context);
		interpreter.run();

	} else {

		std::ifstream file(filename.c_str(), std::ios::binary);
		const Scanner scanner(file, tab_size);
		const Parser parser(scanner, indent_mode);
		Interpreter interpreter(parser, std::cout);
		interpreter.context.head_mode = head_mode;
		interpreter.context.silent_mode = silent_mode;
		interpreter.context.pedantic_mode = pedantic_mode;
		define_input(interpreter.context);
		interpreter.run();

	}

} catch (const std::runtime_error& exception) {

	std::ostringstream message;
	message << (filename != "-" ? "In " + filename : "On standard input")
		<< ":\n" << exception.what();
	throw std::runtime_error(message.str());

}
