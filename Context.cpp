#include "Context.h"
#include "List.h"
#include <iostream>
#include <stdexcept>


Context::Context() : head_mode(false), silent_mode(false), pedantic_mode(false),
	stack{Scope("global")} {}


void Context::define(const Signature& signature,
	std::shared_ptr<const Expression> body, bool allow_redefinition) {

	if (!allow_redefinition) {
		auto position = stack.front().symbols.find(signature);
		if (position != stack.front().symbols.end()) {
			std::ostringstream message;
			message << "Redefinition of \"" << signature.name
				<< "\"";
			if (!stack.front().name.empty())
				message << " in namespace \"" << stack.front().name << "\"";
			message << " already defined as \"" << position->first.canonical
				<< "\".";
			throw std::runtime_error(message.str());
		}
	}

	stack.front().symbols[signature] = body;

	std::string qualified = signature.name;

	auto frame = stack.begin();
	auto previous = frame++;
	while (frame != stack.end() && !previous->name.empty()) {
		qualified = previous->name + "::" + qualified;
		Signature prefixed(qualified, signature);
		frame->symbols[prefixed] = body;
		previous = frame++;
	}

}


void Context::redefine(const Signature& signature,
	std::shared_ptr<const Expression> body) {
	define(signature, body, true);
}


void Context::enter_scope(const std::string& name) {
	stack.push_front(Scope(name));
}


void Context::exit_scope() {
	stack.pop_front();
}


void Context::use(const std::string& prefix) {
	stack.front().use.insert(prefix);
}


std::shared_ptr<const List> Context::evaluate(const std::string& name,
	const std::vector<std::vector<double>>& data,
	const std::vector<std::vector<std::string>>& content) {

	auto scope = stack.begin();
	std::map<Signature, std::shared_ptr<const Expression>>::const_iterator pair;

	while (scope != stack.end()) {
		for (auto prefix = scope->use.begin();
			prefix != scope->use.end(); ++prefix) {
			pair = scope->symbols.begin();
			while (pair != scope->symbols.end()) {
				std::string qualified =
					prefix->empty() ? name : *prefix + "::" + name;
				if (pair->first.matches(qualified, data, content))
					goto found;
				++pair;
			}
		}
		++scope;
	}

	{
		std::ostringstream message;
		message << "Warning: No match for template \"" << name;
		for (auto i = data.begin(); i != data.end(); ++i)
			message << '(' << i->size() << ')';
		for (auto i = content.begin(); i != content.end(); ++i)
			message << '{' << i->size() << '}';
		message << "\".";
		// throw std::runtime_error(message.str());
		std::cerr << message.str() << '\n';
		return std::shared_ptr<const List>(new List(0, 0));
	}

found:

	enter_scope();
	pair->first.bind(*this, data, content);
	std::shared_ptr<const List> result(pair->second->evaluate(*this));
	exit_scope();

	return result;

}
