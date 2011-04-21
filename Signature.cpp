#include "Signature.h"
#include "Content.h"
#include "Context.h"
#include "Data.h"
#include "List.h"
#include <memory>

#include <iostream>


Signature::Signature(const std::string& name) : name(name), canonical(name) {}


/**
 * Initialize the Signature from another, re-constructing the canonical name.
 */
Signature::Signature(const std::string& name, const Signature& signature)
	: name(name), data(signature.data), content(signature.content) {

	std::ostringstream result;
	result << name;

	for (auto i = data.begin(); i != data.end(); ++i) {
		result << '(' << i->first.size();
		if (i->second) result << '+';
		result << ')';
	}

	for (auto i = content.begin(); i != content.end(); ++i) {
		result << '{' << i->first.size();
		if (i->second) result << '+';
		result << '}';
	}

	canonical = result.str();

}


/**
 * Initialize the Signature and construct its canonical name.
 */
Signature::Signature(const std::string& name,
	const std::vector<std::vector<std::string>>& data_names,
	const std::vector<std::vector<std::string>>& content_names) : name(name) {

	std::ostringstream result;
	result << name;

	for (auto i = data_names.begin(); i != data_names.end(); ++i) {
		result << '(' << i->size();
		if (i->back().size() > 3 &&
			i->back().substr(i->back().size() - 3, 3) == "...") {
			data.push_back({*i, true});
			result << '+';
		} else {
			data.push_back({*i, false});
		}
		result << ')';
	}

	for (auto i = content_names.begin(); i != content_names.end(); ++i) {
		result << '{' << i->size();
		if (i->back().size() > 3 &&
			i->back().substr(i->back().size() - 3, 3) == "...") {
			content.push_back({*i, true});
			result << '+';
		} else {
			content.push_back({*i, false});
		}
		result << '}';
	};

	canonical = result.str();

}


/**
 * Inject definitions for each Signature parameter into a Context.
 */
void Signature::bind(Context& context,
	const std::vector<std::vector<double>>& given_data,
	const std::vector<std::vector<std::string>>& given_content) const {

	for (unsigned int section = 0; section < data.size(); ++section) {

		unsigned int element;

		auto& data_section = data[section].first;

		for (element = 0; element < data_section.size() -
			(data[section].second ? 1 : 0); ++element)
			context.define(Signature(data_section[element]),
				std::shared_ptr<const Expression>(new Data
				(0, 0, given_data[section][element])));

		if (data[section].second) {

			std::shared_ptr<List> rest(new List(0, 0));

			while (element < given_data[section].size()) {
				rest->add(std::shared_ptr<const Value>(new Data
					(0, 0, given_data[section][element])));
				++element;
			}

			context.define(Signature
				(data_section[data_section.size() - 1]),
				std::static_pointer_cast<const Expression>(rest));

		}

	}


	for (unsigned int section = 0; section < content.size(); ++section) {

		unsigned int element;

		auto& content_section = content[section].first;

		for (element = 0; element < content_section.size() -
			(content[section].second ? 1 : 0); ++element)
			context.define(Signature(content_section[element]),
				std::shared_ptr<const Expression>(new Content
				(0, 0, given_content[section][element])));

		if (content[section].second) {

			std::shared_ptr<List> rest(new List(0, 0));

			while (element < given_content[section].size()) {
				rest->add(std::shared_ptr<const Value>(new Content
					(0, 0, given_content[section][element])));
				++element;
			}

			context.define(Signature
				(content_section[content_section.size() - 1]),
				std::static_pointer_cast<const Expression>(rest));

		}

	}

}


/**
 * Test whether a given set of values match a Signature.
 */
bool Signature::matches(const std::string& given_name,
	const std::vector<std::vector<double>>& given_data,
	const std::vector<std::vector<std::string>>& given_content) const {

	if (given_name != name ||
		given_data.size() != data.size() ||
		given_content.size() != content.size())
		return false;

	for (unsigned int i = 0; i < data.size(); ++i)
		if (!((data[i].second &&
			given_data[i].size() >= data[i].first.size()) ||
			given_data[i].size() == data[i].first.size()))
			return false;

	for (unsigned int i = 0; i < content.size(); ++i)
		if (!((content[i].second &&
			given_content[i].size() >= content[i].first.size()) ||
			given_content[i].size() == content[i].first.size()))
			return false;

	return true;

}


/**
 * Compare Signatures for sorting in a map.
 */
bool operator<(const Signature& a, const Signature& b) {
	return a.canonical < b.canonical;
}
