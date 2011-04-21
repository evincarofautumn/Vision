#ifndef CONTEXT_H
#define CONTEXT_H
#include "Expression.h"
#include "Signature.h"
#include "Value.h"
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <set>


/**
 * The current execution context and symbol table of an Interpreter.
 */
class Context {
public:

	Context();

	void define(const Signature&, std::shared_ptr<const Expression>,
		bool = false);
	void redefine(const Signature&, std::shared_ptr<const Expression>);

	void enter_scope(const std::string& = "");
	void exit_scope();
	void use(const std::string&);

	std::shared_ptr<const List> evaluate(const std::string&,
		const std::vector<std::vector<double>>& =
		std::vector<std::vector<double>>(),
		const std::vector<std::vector<std::string>>& =
		std::vector<std::vector<std::string>>());

	bool head_mode;
	bool silent_mode;
	bool pedantic_mode;
	std::ostringstream head_buffer;

private:

	struct Scope {

		Scope(const std::string& name = "") : name(name), use{""} {}

		std::string name;
		std::map<Signature, std::shared_ptr<const Expression>> symbols;
		std::set<std::string> use;

	};

	std::list<Scope> stack;


};


#endif

