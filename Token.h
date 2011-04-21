#ifndef TOKEN_H
#define TOKEN_H
#include <iosfwd>
#include <string>


/**
 * A token, with an abstract type, string representation, and file position.
 */
class Token {
public:

	enum Type {

		UNSPECIFIED = 0,
		IDENTIFIER,        // id
		CONTENT,           // "content" 'content' <content>
		DATA,              // 1 2.0
		LEFT_BRACKET,      // [
		RIGHT_BRACKET,     // ]
		LEFT_PARENTHESIS,  // (
		RIGHT_PARENTHESIS, // )
		LEFT_BRACE,        // {
		RIGHT_BRACE,       // }
		SEMICOLON,         // ;
		INDENT,            // ->
		DEDENT,            // <-

	};

	Token();
	Token(Type, int, int);
	Token(Type, const std::string&, int, int);

	operator bool() const;

	Type type;
	const std::string string;
	const int line;
	const int column;

};


std::ostream& operator<<(std::ostream&, Token::Type);


#endif
