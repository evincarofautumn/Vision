#include "Parser.h"
#include "Block.h"
#include "Compound.h"
#include "Content.h"
#include "Data.h"
#include "Group.h"
#include "Identifier.h"
#include "Scanner.h"
#include <sstream>
#include <stdexcept>


Token accept_token
	(const std::list<Token>&, std::list<Token>::const_iterator&, Token::Type);
Token expect_token
	(const std::list<Token>&, std::list<Token>::const_iterator&, Token::Type);
std::shared_ptr<const Expression> accept_expression
	(const std::list<Token>&, std::list<Token>::const_iterator&);
std::shared_ptr<const Expression> expect_expression
	(const std::list<Token>&, std::list<Token>::const_iterator&);


Parser::Parser(const Scanner& scanner, bool indent_mode) : scanner(scanner),
	indent_mode(indent_mode) {}


Token accept_token(const std::list<Token>& tokens,
	std::list<Token>::const_iterator& current, Token::Type type) {

	if (current == tokens.end() || current->type != type)
		return Token();

	return *current++;

}


Token expect_token(const std::list<Token>& tokens,
	std::list<Token>::const_iterator& current, Token::Type type) {

	const Token token = accept_token(tokens, current, type);

	if (!token) {
		std::ostringstream message;
		message << "Expected " << type;
		if (current != tokens.end())
			message << " before " << current->type;
		message << ".";
		throw std::runtime_error(message.str());
	}

	return token;

}


std::shared_ptr<const Expression> accept_expression
	(const std::list<Token>& tokens,
	std::list<Token>::const_iterator& current) {

	std::shared_ptr<Expression> result;
	std::shared_ptr<const Expression> expression;

	// A number never names a thing of meaning.
	// "I am not a number, I am a free man!"
	if (Token token = accept_token(tokens, current, Token::DATA)) {

		std::istringstream stream(token.string);
		double data;
		stream >> data;
		return std::shared_ptr<const Expression>(new Data
			(token.line, token.column, data));

	}

	// id
	if (Token token = accept_token(tokens, current, Token::IDENTIFIER)) {

		result.reset(new Identifier(token.line, token.column, token.string));

	// "content"
	} else if (Token token = accept_token(tokens, current, Token::CONTENT)) {

		result.reset(new Content(token.line, token.column, token.string));

	// [...]
	} else if (Token token = accept_token
		(tokens, current, Token::LEFT_BRACKET)) {

		std::shared_ptr<Expression> block(new Block(token.line, token.column));

		do {

			expression = expect_expression(tokens, current);
			std::static_pointer_cast<Block>(block)->add(expression);
			if (current == tokens.end()) {
				std::ostringstream message;
				message << "Unexpected end of file in block beginning at line "
					<< token.line << ", column " << token.column << ".";
				throw std::runtime_error(message.str());
			}

		} while (!accept_token(tokens, current, Token::RIGHT_BRACKET));

		result = block;

	// {...}
	} else if (Token token = accept_token
		(tokens, current, Token::LEFT_BRACE)) {

		std::shared_ptr<Expression> block(new Block(token.line, token.column));

		do {

			expression = expect_expression(tokens, current);
			std::static_pointer_cast<Block>(block)->add(expression);
			if (current == tokens.end()) {
				std::ostringstream message;
				message << "Unexpected end of file in block beginning at line "
					<< token.line << ", column " << token.column << ".";
				throw std::runtime_error(message.str());
			}

		} while (!accept_token(tokens, current, Token::RIGHT_BRACE));

		result = block;

	// (...)
	} else if (Token token = accept_token
		(tokens, current, Token::LEFT_PARENTHESIS)) {

		std::shared_ptr<Expression> group(new Group(token.line, token.column));

		do {

			expression = expect_expression(tokens, current);
			std::static_pointer_cast<Group>(group)->add(expression);
			if (current == tokens.end()) {
				std::ostringstream message;
				message << "Unexpected end of file in group beginning at line "
					<< token.line << ", column " << token.column << ".";
				throw std::runtime_error(message.str());
			}

		} while (!accept_token(tokens, current, Token::RIGHT_PARENTHESIS));

		result = group;

	// \(o_O)/
	} else {

		return std::shared_ptr<const Expression>();

	}

	if (current == tokens.end() ||
		accept_token(tokens, current, Token::SEMICOLON))
		return result;

	if (current->type == Token::LEFT_BRACKET ||
		current->type == Token::LEFT_PARENTHESIS ||
		current->type == Token::LEFT_BRACE) {

		std::shared_ptr<Expression> compound(new Compound
			(result->line_number, result->column_number));
		std::static_pointer_cast<Compound>(compound)->set_determiner(result);
		result = compound;

	}

	// [id]
	if (accept_token(tokens, current, Token::LEFT_BRACKET)) {
		std::static_pointer_cast<Compound>(result)->set_identifier
			(expect_token(tokens, current, Token::IDENTIFIER).string);
		expect_token(tokens, current, Token::RIGHT_BRACKET);
	}

	// (...)
	while (Token token = accept_token
		(tokens, current, Token::LEFT_PARENTHESIS)) {

		std::static_pointer_cast<Compound>(result)->add_data();

		do {

			expression = expect_expression(tokens, current);
			std::static_pointer_cast<Compound>(result)->add_data(expression);

			if (current == tokens.end()) {
				std::ostringstream message;
				message << "Unexpected end of file in data block "
					"beginning at line " << token.line << ", column "
					<< token.column << ".";
				throw std::runtime_error(message.str());
			}

		} while (!accept_token(tokens, current, Token::RIGHT_PARENTHESIS));

	}

	// {...}
	while (Token token = accept_token(tokens, current, Token::LEFT_BRACE)) {

		std::static_pointer_cast<Compound>(result)->add_content();

		do {

			expression = expect_expression(tokens, current);
			std::static_pointer_cast<Compound>
				(result)->add_content(expression);

			if (current == tokens.end()) {
				std::ostringstream message;
				message << "Unexpected end of file in content block "
					"beginning at line " << token.line << ", column "
					<< token.column << ".";
				throw std::runtime_error(message.str());
			}

		} while (!accept_token(tokens, current, Token::RIGHT_BRACE));

	}

	// ;
	accept_token(tokens, current, Token::SEMICOLON);

	return std::static_pointer_cast<const Expression>(result);

}


std::shared_ptr<const Expression> expect_expression
	(const std::list<Token>& tokens,
	std::list<Token>::const_iterator& current) {

	std::shared_ptr<const Expression> expression =
		accept_expression(tokens, current);

	if (!expression) {
		std::ostringstream message;
		message << "Expected expression before ";
		if (current != tokens.end())
			message << current->type;
		else
			message << "end of file";
		message << ".";
		throw std::runtime_error(message.str());
	}

	return expression;

}


void expect_balanced(const std::list<Token>& tokens) {

	std::list<std::list<Token>::const_iterator> delimiters;

	auto token = tokens.begin();
	while (token != tokens.end()) {
		if (token->type == Token::INDENT ||
			token->type == Token::LEFT_PARENTHESIS ||
			token->type == Token::LEFT_BRACKET ||
			token->type == Token::LEFT_BRACE) {
			delimiters.push_back(token);
		} else if (token->type == Token::DEDENT) {
			if (!delimiters.empty() &&
				delimiters.back()->type == Token::INDENT)
				delimiters.pop_back();
			else
				break;
		} else if (token->type == Token::RIGHT_PARENTHESIS) {
			if (!delimiters.empty() &&
				delimiters.back()->type == Token::LEFT_PARENTHESIS)
				delimiters.pop_back();
			else
				break;
		} else if (token->type == Token::RIGHT_BRACKET) {
			if (!delimiters.empty() &&
				delimiters.back()->type == Token::LEFT_BRACKET)
				delimiters.pop_back();
			else
				break;
		} else if (token->type == Token::RIGHT_BRACE) {
			if (!delimiters.empty() &&
				delimiters.back()->type == Token::LEFT_BRACE)
				delimiters.pop_back();
			else
				break;
		}
		++token;
	}

	if (token == tokens.end()) {

		if (!delimiters.empty()) {

			std::ostringstream message;
			message << delimiters.back()->type
				<< " at line " << delimiters.back()->line
				<< ", column " << delimiters.back()->column
				<< " has no match.";
			throw std::runtime_error(message.str());

		}

	} else {

		std::ostringstream message;
		message << token->type
			<< " at line " << token->line
			<< ", column " << token->column;

		if (delimiters.empty()) {

			message << " has no match.";

		} else {

			message << " does not match " << delimiters.back()->type
				<< " at line " << delimiters.back()->line
				<< ", column " << delimiters.back()->column
				<< ".";

		}

		throw std::runtime_error(message.str());

	}

}


std::shared_ptr<const Expression> Parser::run() const {

	std::list<Token> tokens = scanner.run();
	std::list<Token>::const_iterator current = tokens.begin();
	std::shared_ptr<Block> expressions(new Block(0, 0));

	expect_balanced(tokens);

	if (indent_mode) {
		for (auto i = tokens.begin(); i != tokens.end(); ++i) {
			if (i->type == Token::INDENT)
				i->type = Token::LEFT_BRACE;
			else if (i->type == Token::DEDENT)
				i->type = Token::RIGHT_BRACE;
		}
	} else {
		auto i = tokens.begin();
		while (i != tokens.end()) {
			if (i->type == Token::INDENT || i->type == Token::DEDENT)
				i = tokens.erase(i);
			else
				++i;
		}
	}

	try {

		while (std::shared_ptr<const Expression> expression =
			accept_expression(tokens, current))
			expressions->add(expression);

	} catch (const std::runtime_error& exception) {

		std::ostringstream message;
		message << "At ";

		if (current != tokens.end())
			message << "line " << current->line << ", column "
				<< current->column;
		else
			message << "end of file";

		message << ":\n" << exception.what();

		throw std::runtime_error(message.str());

	}

	return std::static_pointer_cast<const Expression>(expressions);

}
