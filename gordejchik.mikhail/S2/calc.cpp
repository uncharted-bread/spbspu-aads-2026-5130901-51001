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

static bool isUnaryOp(const std::string& token)
{
  return token == "!";
}

static int getPrecedence(const std::string& op)
{
  if (op == "!") {
    return 4;
  }
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
  return op != "!";
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
    const size_t start = i;
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
    } else if (isBinaryOp(token) || isUnaryOp(token)) {
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

static long long applyBinary(long long left, long long right,
    const std::string& op)
{
  if (op == "+") {
    return left + right;
  }
  if (op == "-") {
    return left - right;
  }
  if (op == "*") {
    return left * right;
  }
  if (op == "/") {
    if (right == 0) {
      throw std::invalid_argument("Деление на 0");
    }
    return left / right;
  }
  if (op == "%") {
    if (right == 0) {
      throw std::invalid_argument("Взятие остатка от деления на 0");
    }
    return left % right;
  }
  throw std::invalid_argument("Неизвестный оператор " + op);
}

static long long evaluatePostfix(gordejchik::Queue< std::string >& postfix)
{
  gordejchik::Stack< long long > operands;

  while (!postfix.empty()) {
    const std::string token = postfix.front();
    postfix.pop();

    if (isNumber(token)) {
      operands.push(std::stoll(token));
    } else if (isUnaryOp(token)) {
      if (operands.empty()) {
        throw std::invalid_argument("Недостаточно опрерандов for !");
      }
      const long long val = operands.top();
      operands.pop();
      operands.push(~val);
    } else if (isBinaryOp(token)) {
      if (operands.size() < 2) {
        throw std::invalid_argument("Недостаточно опрерандов");
      }
      const long long right = operands.top();
      operands.pop();
      const long long left = operands.top();
      operands.pop();
      operands.push(applyBinary(left, right, token));
    } else {
      throw std::invalid_argument("Неверный токен постфикса: " + token);
    }
  }

  if (operands.size() != 1) {
    throw std::invalid_argument("Неверное выражение");
  }
  return operands.top();
}

long long gordejchik::calculateExpression(const std::string& line)
{
  gordejchik::Queue< std::string > tokens = tokenize(line);
  if (tokens.empty()) {
    throw std::invalid_argument("Пустое выражение");
  }
  gordejchik::Queue< std::string > postfix = infixToPostfix(tokens);
  return evaluatePostfix(postfix);
}
