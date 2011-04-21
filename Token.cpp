#include "Token.h"
#include <iostream>


Token::Token() : type(UNSPECIFIED), line(0), column(0) {}


Token::Token(Token::Type type, int line, int column) : type(type), line(line),
	column(column) {}


Token::Token(Token::Type type, const std::string& string, int line,
	int column) : type(type), string(string), line(line), column(column) {}


/**
 * Test the Token for truthiness according to whether its type is specified.
 */
Token::operator bool() const { return type; }


/**
 * Output the type of a Token as a pretty(ish) string.
 */
std::ostream& operator<<(std::ostream& stream, Token::Type type) {

	switch (type) {
	case Token::IDENTIFIER:        return stream << "identifier";
	case Token::CONTENT:           return stream << "content";
	case Token::DATA:              return stream << "data";
	case Token::LEFT_BRACKET:      return stream << "\"[\"";
	case Token::RIGHT_BRACKET:     return stream << "\"]\"";
	case Token::LEFT_PARENTHESIS:  return stream << "\"(\"";
	case Token::RIGHT_PARENTHESIS: return stream << "\")\"";
	case Token::LEFT_BRACE:        return stream << "\"{\"";
	case Token::RIGHT_BRACE:       return stream << "\"}\"";
	case Token::SEMICOLON:         return stream << "\";\"";
	case Token::INDENT:            return stream << "indent";
	case Token::DEDENT:            return stream << "dedent";
	default:                       return stream;
	}

}
