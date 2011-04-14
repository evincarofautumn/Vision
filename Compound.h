#ifndef COMPOUND_H
#define COMPOUND_H
#include "Expression.h"
#include <map>
#include <memory>
#include <vector>


class Compound : public Expression {
public:

	Compound(int, int);
	virtual ~Compound();

	void set_determiner(std::shared_ptr<const Expression>);
	void set_identifier(const std::string&);

	void add_data();
	void add_data(std::shared_ptr<const Expression>);
	void add_content();
	void add_content(std::shared_ptr<const Expression>);

	virtual std::shared_ptr<const List> evaluate(Context&) const;
	virtual std::string get_content() const;
	virtual double get_data() const;

	typedef double(MathFunction)(const std::vector<double>&);

private:

	typedef std::shared_ptr<const List>(Evaluator)
		(const std::string&, Context&) const;
	typedef std::shared_ptr<const List>
		(Compound::*EvaluatorPointer)(const std::string&, Context&) const;
	typedef double(*MathFunctionPointer)(const std::vector<double>&);

	Evaluator evaluate_def;
	Evaluator evaluate_error;
	Evaluator evaluate_extern;
	Evaluator evaluate_file;
	Evaluator evaluate_header;
	Evaluator evaluate_if;
	Evaluator evaluate_local;
	Evaluator evaluate_math;
	Evaluator evaluate_namespace;
	Evaluator evaluate_use;
	Evaluator evaluate_using;
	Evaluator evaluate_warn;

	std::shared_ptr<const Expression> determiner;
	std::string identifier;
	std::vector<std::vector<std::shared_ptr<const Expression>>> data;
	std::vector<std::vector<std::shared_ptr<const Expression>>> content;

	static bool is_keyword(const std::string&);

	static std::map<std::string, EvaluatorPointer> evaluators;
	static std::map<std::string, int> math_arities;
	static std::map<std::string, MathFunctionPointer> math_functions;

};


#endif
