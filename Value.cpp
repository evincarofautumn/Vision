#include "Value.h"


Value::Value(int line, int column) : Expression(line, column) {}


Value::~Value() {}


/**
 * Return a reference to this Value, or else a reference to a copy of it.
 */
std::shared_ptr<const Value> Value::self_reference() const try {

	return shared_from_this();

} catch (const std::bad_weak_ptr&) {

	return std::shared_ptr<const Value>(clone());

}
