#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
  // TODO: initialize member variable(s) (if necessary)
{
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  // TODO: implement
  return stack.empty();
}

void ValueStack::push( const std::string &value )
{
  // TODO: implement
  stack.push_back(value);
}

std::string ValueStack::get_top() const
{
  // TODO: implement
  if (stack.empty()) {
    throw OperationException("called get_top() when stack is empty");
  }
  return stack.back();
}

void ValueStack::pop()
{
  // TODO: implement
  if (stack.empty()) {
    throw OperationException("called pop() when stack is empty");
  }
  return stack.pop_back();
}
