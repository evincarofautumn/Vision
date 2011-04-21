#include "Scanner.h"
#include <cstdint>
#include <sstream>
#include <vector>
#include <utf8.h>


Scanner::Scanner(std::istream& stream, int tab_size) : stream(stream),
	tab_size(tab_size) {}


/**
 * Run the Scanner. This probably ought to be split up a bit, but it's very
 * straightforward: continually get the next UTF-8 character from the stream,
 * and jump around between states accordingly. Everything that can go wrong
 * probably has an associated error message that's reasonably easy to read.
 */
std::list<Token> Scanner::run() const {

	std::list<Token> result;
	int file_line = 1;
	int file_column = 1;
	auto tell = std::istreambuf_iterator<char>(stream);
	auto end = std::istreambuf_iterator<char>();

	try {

		uint32_t current = 0;           // Current (UTF-32) character.
		uint32_t previous = 0;          // Previous character.
		int token_line = 0;             // Line number of current token.
		int token_column = 0;           // Column number of current token.
		int indent = 0;                 // Current indent level.
		bool need = true;               // Whether we need a new character.
		bool done = false;              // Whether we're done scanning.
		bool in_indent = true;          // Whether we're in the indent.
		std::vector<int> indents{0};    // Indent level stack.
		std::string string;             // String of current token.
		std::string heredoc_begin;      // Heredoc begin identifier.
		std::string heredoc_indent;     // Heredoc indent string.
		std::string heredoc_end;        // Heredoc end identifier.

		enum State {

			NORMAL = 0,        // Default state, without expectations.
			IDENTIFIER,        // Within an identifier.
			DOUBLE,            // Within a double-quoted string.
			SINGLE,            // Within a single-quoted string.
			HEREDOC_BEGIN,     // At the beginning of a heredoc.
			HEREDOC,           // In the body of a heredoc.
			HEREDOC_INDENT,    // Possibly in the final indent of a heredoc.
			HEREDOC_END,       // At the end of a heredoc.
			INTEGER,           // Within an integral constant.
			FRACTION,          // Within a fractional constant.
			COMMENT_BEGIN,     // At the beginning of a comment.
			COMMENT,           // Within the body of a single-line comment.
			COMMENT_BLOCK,     // Within the body of a comment block.

		} state = NORMAL;

		while (!done) {

			if (need) {
				previous = current;
				if (tell != end) {
					current = utf8::next(tell, end);
					if (current == '\n') {
						file_column = 0;
						in_indent = true;
						indent = 0;
						++file_line;
					} else if (current == '\t') {
						file_column += tab_size - file_column % tab_size;
						if (in_indent) indent += tab_size;
					} else if (std::isspace(current)) {
						++file_column;
						if (in_indent) ++indent;
					} else {
						++file_column;
					}
				} else {
					done = true;
					current = '\n';
				}
			}

			need = true;

			switch (state) {

			case NORMAL:

				token_line = file_line;
				token_column = file_column;

				// Here's where the indentation magic happens.

				if (in_indent && !std::isspace(current)) {
					in_indent = false;
					if (indent > indents.back()) {
						indents.push_back(indent);
						result.push_back(Token(Token::INDENT, token_line,
							token_column));
					} else {
						while (!indents.empty() && indent != indents.back()) {
							indents.pop_back();
							result.push_back(Token(Token::DEDENT, token_line,
								token_column));
						}
						if (indents.empty())
							throw std::runtime_error
								("Invalid indentation level.");
					}
				}

				switch (current) {

				// A lot of boring and obvious stuff starts here.

				case '[':
					result.push_back(Token(Token::LEFT_BRACKET, token_line,
						token_column));
					break;

				case ']':
					result.push_back(Token(Token::RIGHT_BRACKET, token_line,
						token_column));
					break;

				case '(':
					result.push_back(Token(Token::LEFT_PARENTHESIS, token_line,
						token_column));
					break;

				case ')':
					result.push_back(Token(Token::RIGHT_PARENTHESIS, token_line,
						token_column));
					break;

				case '{':
					result.push_back(Token(Token::LEFT_BRACE, token_line,
						token_column));
					break;

				case '}':
					result.push_back(Token(Token::RIGHT_BRACE, token_line,
						token_column));
					break;

				case ';':
					result.push_back(Token(Token::SEMICOLON, token_line,
						token_column));
					break;

				case '"':
					state = DOUBLE;
					break;

				case '\'':
					state = SINGLE;
					break;

				case '<':
					heredoc_begin.clear();
					state = HEREDOC_BEGIN;
					break;

				case '#':
					state = COMMENT_BEGIN;
					break;

				default:
					if (std::isspace(current)) {
						// Space is like silence, the written lack of action.
					} else if (std::isdigit(current)) {
						string += current;
						state = INTEGER;
					} else {
						string += current;
						state = IDENTIFIER;
					}
					break;
				}
				break;

			case COMMENT_BEGIN:
				switch (current) {
				case '\n':
					state = NORMAL;
					break;
				case ':':
					state = COMMENT_BLOCK;
					break;
				default:
					state = COMMENT;
				}
				break;

			case COMMENT:
				if (current == '\n')
					state = NORMAL;
				break;

			case COMMENT_BLOCK:
				if (current == '#' && previous == ':')
					state = NORMAL;
				break;

			case IDENTIFIER:

				// Identifiers can't contain these, for obvious reasons.
				if (std::string("'\"<[](){};#").find(current) !=
					std::string::npos || std::isspace(current)) {
					result.push_back(Token(Token::IDENTIFIER, string,
						token_line, token_column));
					string.clear();
					need = false;
					state = NORMAL;
				} else {
					string += current;
				}
				break;

			case DOUBLE:
				if (current == '"') {
					if (previous == '\\') {
						string += current;
					} else {
						result.push_back(Token(Token::CONTENT, string,
							token_line, token_column));
						string.clear();
						state = NORMAL;
					}
				} else if (current == '\\') {
					if (previous == '\\') {
						string += current;
					}
				} else {
					string += current;
				}
				break;

			case SINGLE:
				if (current == '\'') {
					if (previous == '\\') {
						string += current;
					} else {
						result.push_back(Token(Token::CONTENT, string,
							token_line, token_column));
						string.clear();
						state = NORMAL;
					}
				} else if (current == '\\') {
					if (previous == '\\') {
						string += current;
					}
				} else {
					string += current;
				}
				break;

			case HEREDOC_BEGIN:
				if (std::isalpha(current)) {
					heredoc_begin += current;
				} else if (current == '\n') {
					state = HEREDOC;
				} else if (current == '\r') {
					// Just in case.
				} else {
					throw std::runtime_error("Invalid heredoc identifier.");
				}
				break;

			case HEREDOC:
				if (current == '\n') {
					heredoc_indent.clear();
					state = HEREDOC_INDENT;
				} else {
					string += current;
				}
				break;

			case HEREDOC_INDENT:
				// It's not the end after all!
				if (current == '\n') {
					string += '\n';
					string += heredoc_indent;
					heredoc_indent.clear();
				} else if (std::isspace(current)) {
					heredoc_indent += current;
				} else if (std::isalpha(current)) {
					need = false;
					heredoc_end.clear();
					state = HEREDOC_END;
				} else if (current == '>' && heredoc_begin.empty()) {
					result.push_back(Token(Token::CONTENT, string, token_line,
						token_column));
					string.clear();
					state = NORMAL;
				} else {
					string += '\n';
					string += heredoc_indent;
					string += current;
					state = HEREDOC;
				}
				break;

			case HEREDOC_END:
				if (current == '>' && heredoc_end == heredoc_begin) {
					result.push_back(Token(Token::CONTENT, string,
						token_line, token_column));
					string.clear();
					state = NORMAL;
				} else if (std::isalpha(current)) {
					heredoc_end += current;
				} else if (current == '\n') {
					string += '\n';
					string += heredoc_indent;
					string += heredoc_end;
					heredoc_indent.clear();
					state = HEREDOC_INDENT;
				} else {
					string += '\n';
					string += heredoc_indent;
					string += heredoc_end;
					string += current;
					state = HEREDOC;
				}
				break;

			case INTEGER:
				if (std::isdigit(current)) {
					string += current;
				} else if (current == '.') {
					string += current;
					state = FRACTION;
				} else {
					result.push_back(Token(Token::DATA, string,
						token_line, token_column));
					string.clear();
					need = false;
					state = NORMAL;
				}
				break;

			case FRACTION:
				// I know, I didn't do anything interesting here.
				// But do you really need scientific notation?
				if (std::isdigit(current)) {
					string += current;
				} else {
					if (string[string.size() - 1] == '.') {
						throw std::runtime_error
							("Invalid floating-point number.");
					} else {
						result.push_back(Token(Token::DATA, string,
							token_line, token_column));
						string.clear();
						need = false;
						state = NORMAL;
					}
				}
				break;

			}

		}

		switch (state) {

		case DOUBLE:
			throw std::runtime_error
				("Unexpected end of file in double-quoted string.");

		case SINGLE:
			throw std::runtime_error
				("Unexpected end of file in single-quoted string.");

		case HEREDOC:
		case HEREDOC_INDENT:
		case HEREDOC_END:
			throw std::runtime_error
				("Unexpected end of file in heredoc.");

		default:
			break;

		}

		for (unsigned int i = 0; i < indents.size() - 1; ++i)
			result.push_back(Token(Token::DEDENT, file_line, file_column));

	} catch (const std::runtime_error& exception) {

		// Error messages have this thing where it's important that you know
		// where the hell they're coming from. I made that happen here.

		std::ostringstream message;
		message << "At ";
		if (tell != end)
			message << "line " << file_line << ", column " << file_column;
		else
			message << "end of file";
		message << ":\n" << exception.what();
		throw std::runtime_error(message.str());

	}

	// When you have run as far as you can,
	// you will find that all you can do
	// is return.
	return result;

}
