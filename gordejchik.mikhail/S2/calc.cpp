#include "calc.hpp"
#include <string>
#include <stdexcept>
#include "stack.hpp"
#include "queue.hpp"

static bool isBinaryOp(const std::string& token)
{
  return (token == "+")
      || (token == "-")
      || (token == "*")
      || (token == "/")
      || (token == "%");
}

static int getPrecedence(const std::string& op)
{
  if ((op == "*") || (op == "/") || (op == "%")) {
    return 3;
  }
  if ((op == "+") || (op == "-")) {
    return 2;
  }
  return 0;
}

static bool isLeftAssociative(const std::string& op)
{
  (void)op;
  return true;
}

static bool isNumber(const std::string& token)
{
  if (token.empty()) {
    return false;
  }
  size_t start = 0;
  if ((token[0] == '-') || (token[0] == '+')) {
    if (token.size() == 1) {
      return false;
    }
    start = 1;
  }
  for (size_t i = start; i < token.size(); ++i) {
    if ((token[i] < '0') || (token[i] > '9')) {
      return false;
    }
  }
  return true;
}

static gordejchik::Queue< std::string > tokenize(const std::string& line)
{
  gordejchik::Queue< std::string > tokens;
  size_t i = 0;
  while (i < line.size()) {
    while ((i < line.size()) && (line[i] == ' ')) {
      ++i;
    }
    if (i >= line.size()) {
      break;
    }
    size_t start = i;
    while ((i < line.size()) && (line[i] != ' ')) {
      ++i;
    }
    tokens.push(line.substr(start, i - start));
  }
  return tokens;
}

static gordejchik::Queue< std::string > infixToPostfix(
    gordejchik::Queue< std::string >& input)
{
  gordejchik::Queue< std::string > output;
  gordejchik::Stack< std::string > ops;

  while (!input.empty()) {
    const std::string token = input.front();
    input.pop();

    if (isNumber(token)) {
      output.push(token);
    } else if (isBinaryOp(token)) {
      while (!ops.empty() && (ops.top() != "(")) {
        bool shouldPop = false;
        if (isLeftAssociative(token)) {
          shouldPop = getPrecedence(ops.top()) >= getPrecedence(token);
        } else {
          shouldPop = getPrecedence(ops.top()) > getPrecedence(token);
        }
        if (!shouldPop) {
          break;
        }
        output.push(ops.top());
        ops.pop();
      }
      ops.push(token);
    } else if (token == "(") {
      ops.push(token);
    } else if (token == ")") {
      while (!ops.empty() && (ops.top() != "(")) {
        output.push(ops.top());
        ops.pop();
      }
      if (ops.empty()) {
        throw std::invalid_argument("Несовпадающие скобки");
      }
      ops.pop();
    } else {
      throw std::invalid_argument("Неизвестный токен: " + token);
    }
  }

  while (!ops.empty()) {
    if (ops.top() == "(") {
      throw std::invalid_argument("Несовпадающие скобки");
    }
    output.push(ops.top());
    ops.pop();
  }

  return output;
}

long long gordejchik::calculateExpression(const std::string& line)
{
  gordejchik::Queue< std::string > tokens = tokenize(line);
  if (tokens.empty()) {
    throw std::invalid_argument("Пустое выражение");
  }
  gordejchik::Queue< std::string > postfix = infixToPostfix(tokens);
  (void)postfix;
  throw std::logic_error("Evaluation not yet implemented");
}
