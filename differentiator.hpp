#ifndef DIFFERENTIATOR_HPP
#define DIFFERENTIATOR_HPP

#include <cmath>
#include <complex>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

enum class ExprType {
  Constant,
  Variable,
  Add,
  Sub,
  Mul,
  Div,
  Pow,
  Sin,
  Cos,
  Ln,
  Exp
};

template <typename T> class Expression;

template <typename T> struct ExprNode {
  ExprType type;
  T value;
  std::string varName;

  std::shared_ptr<ExprNode<T>> left;
  std::shared_ptr<ExprNode<T>> right;

  ExprNode(ExprType t, const T &val)
      : type(t), value(val), left(nullptr), right(nullptr) {}

  ExprNode(ExprType t, const std::string &var)
      : type(t), varName(var), left(nullptr), right(nullptr) {}

  ExprNode(ExprType t, std::shared_ptr<ExprNode<T>> l,
           std::shared_ptr<ExprNode<T>> r)
      : type(t), left(l), right(r) {}
};

template <typename T> class Expression {
private:
  std::shared_ptr<ExprNode<T>> root;

public:
  Expression() : root(nullptr) {}

  ~Expression() = default;

  Expression(const Expression &other) : root(other.root) {}

  Expression(Expression &&other) noexcept : root(std::move(other.root)) {}

  Expression &operator=(const Expression &other) {
    if (this != &other) {
      root = other.root;
    }
    return *this;
  }

  Expression &operator=(Expression &&other) noexcept {
    if (this != &other) {
      root = std::move(other.root);
    }
    return *this;
  }

  Expression(const T &val) {
    root = std::make_shared<ExprNode<T>>(ExprType::Constant, val);
  }

  Expression(const std::string &varName) {
    root = std::make_shared<ExprNode<T>>(ExprType::Variable, varName);
  }

  Expression(std::shared_ptr<ExprNode<T>> node) : root(node) {}

  bool isValid() const { return (bool)root; }

  friend Expression<T> operator+(const Expression<T> &lhs,
                                 const Expression<T> &rhs) {
    auto node =
        std::make_shared<ExprNode<T>>(ExprType::Add, lhs.root, rhs.root);
    return Expression<T>(node);
  }

  friend Expression<T> operator-(const Expression<T> &lhs,
                                 const Expression<T> &rhs) {
    auto node =
        std::make_shared<ExprNode<T>>(ExprType::Sub, lhs.root, rhs.root);
    return Expression<T>(node);
  }

  friend Expression<T> operator*(const Expression<T> &lhs,
                                 const Expression<T> &rhs) {
    auto node =
        std::make_shared<ExprNode<T>>(ExprType::Mul, lhs.root, rhs.root);
    return Expression<T>(node);
  }

  friend Expression<T> operator/(const Expression<T> &lhs,
                                 const Expression<T> &rhs) {
    auto node =
        std::make_shared<ExprNode<T>>(ExprType::Div, lhs.root, rhs.root);
    return Expression<T>(node);
  }

  friend Expression<T> operator^(const Expression<T> &lhs,
                                 const Expression<T> &rhs) {
    auto node =
        std::make_shared<ExprNode<T>>(ExprType::Pow, lhs.root, rhs.root);
    return Expression<T>(node);
  }

  friend Expression<T> sin(const Expression<T> &arg) {
    auto node = std::make_shared<ExprNode<T>>(ExprType::Sin, arg.root, nullptr);
    return Expression<T>(node);
  }

  friend Expression<T> cos(const Expression<T> &arg) {
    auto node = std::make_shared<ExprNode<T>>(ExprType::Cos, arg.root, nullptr);
    return Expression<T>(node);
  }

  friend Expression<T> ln(const Expression<T> &arg) {
    auto node = std::make_shared<ExprNode<T>>(ExprType::Ln, arg.root, nullptr);
    return Expression<T>(node);
  }

  friend Expression<T> exp(const Expression<T> &arg) {
    auto node = std::make_shared<ExprNode<T>>(ExprType::Exp, arg.root, nullptr);
    return Expression<T>(node);
  }

  std::string toString() const { return toStringImpl(root); }

  Expression<T> substitute(const std::string &varName, const T &val) const {
    auto newRoot = substituteImpl(root, varName, val);
    return Expression<T>(newRoot);
  }

  T evaluate(const std::map<std::string, T> &varValues = {}) const {
    return evaluateImpl(root, varValues);
  }

  static Expression<T> parse(const std::string &exprStr) {
    std::string s = trim(exprStr);
    if (s.empty()) {
      throw std::runtime_error("Empty expression passed to parse()");
    }

    if (isWrappedInParens(s)) {

      return parse(s.substr(1, s.size() - 2));
    }

    if (s == "pi") {
      return Expression<T>((T)M_PI);
    }
    if (s == "e") {
      return Expression<T>((T)M_E);
    }

    if (s[0] == '-') {

      if (s.size() == 1) {
        throw std::runtime_error("Invalid expression: just '-'.");
      }

      return parse("0") - parse(s.substr(1));
    }

    {
      int parenCount = 0;
      for (std::string::size_type i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '(')
          parenCount++;
        else if (c == ')')
          parenCount--;

        if (parenCount == 0 && (c == '+' || c == '-') && i > 0) {

          std::string left = s.substr(0, i);
          std::string right = s.substr(i + 1);
          if (c == '+') {
            return parse(left) + parse(right);
          } else {
            return parse(left) - parse(right);
          }
        }
      }
    }

    {
      int parenCount = 0;
      for (std::string::size_type i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '(')
          parenCount++;
        else if (c == ')')
          parenCount--;

        if (parenCount == 0 && (c == '*' || c == '/')) {
          std::string left = s.substr(0, i);
          std::string right = s.substr(i + 1);
          if (c == '*') {
            return parse(left) * parse(right);
          } else {
            return parse(left) / parse(right);
          }
        }
      }
    }

    {
      int parenCount = 0;
      for (std::string::size_type i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '(')
          parenCount++;
        else if (c == ')')
          parenCount--;

        if (parenCount == 0 && c == '^') {
          std::string left = s.substr(0, i);
          std::string right = s.substr(i + 1);
          return parse(left) ^ parse(right);
        }
      }
    }

    {
      auto startsWith = [&](const std::string &prefix) {
        return s.size() >= prefix.size() &&
               s.compare(0, prefix.size(), prefix) == 0;
      };

      if (startsWith("sin(")) {
        std::string inside = getInsideFunction(s, "sin");
        return sin(parse(inside));
      }
      if (startsWith("cos(")) {
        std::string inside = getInsideFunction(s, "cos");
        return cos(parse(inside));
      }
      if (startsWith("ln(")) {
        std::string inside = getInsideFunction(s, "ln");
        return ln(parse(inside));
      }
      if (startsWith("exp(")) {
        std::string inside = getInsideFunction(s, "exp");
        return exp(parse(inside));
      }
    }

    try {
      T val = stringToValue<T>(s);
      return Expression<T>(val);
    } catch (...) {

      return Expression<T>(s);
    }
  }

  Expression<T> differentiate(const std::string &varName) const {
    auto diffRoot = differentiateImpl(root, varName);
    return Expression<T>(diffRoot);
  }

private:
  template <typename U> static U stringToValue(const std::string &s) {
    std::stringstream ss(s);
    U val;
    ss >> val;

    if (ss.fail() || !ss.eof()) {
      throw std::runtime_error("Cannot parse to numeric value: " + s);
    }
    return val;
  }

  static std::string trim(const std::string &s) {
    const char *ws = " \t\n\r";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos)
      return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
  }

  static bool isWrappedInParens(const std::string &s) {
    if (s.size() < 2)
      return false;
    if (s.front() != '(' || s.back() != ')')
      return false;

    int depth = 0;
    for (size_t i = 0; i < s.size() - 1; i++) {
      if (s[i] == '(')
        depth++;
      if (s[i] == ')')
        depth--;

      if (depth == 0 && i < s.size() - 1) {
        return false;
      }
    }

    return true;
  }

  static std::string getInsideFunction(const std::string &expr,
                                       const std::string &funcName) {
    auto startPos = expr.find(funcName + "(");
    if (startPos == std::string::npos) {
      throw std::runtime_error("parse error with function " + funcName);
    }
    startPos += funcName.size() + 1;
    int parenCount = 1;
    size_t i = startPos;
    for (; i < expr.size(); ++i) {
      if (expr[i] == '(')
        parenCount++;
      if (expr[i] == ')')
        parenCount--;
      if (parenCount == 0) {

        break;
      }
    }
    if (parenCount != 0) {
      throw std::runtime_error("Unmatched parentheses in function call: " +
                               expr);
    }

    return expr.substr(startPos, i - startPos);
  }

  static std::string toStringImpl(const std::shared_ptr<ExprNode<T>> &node) {
    if (!node)
      return "";
    switch (node->type) {
    case ExprType::Constant: {
      std::ostringstream oss;
      oss << node->value;
      return oss.str();
    }
    case ExprType::Variable:
      return node->varName;
    case ExprType::Add:
      return "(" + toStringImpl(node->left) + " + " +
             toStringImpl(node->right) + ")";
    case ExprType::Sub:
      return "(" + toStringImpl(node->left) + " - " +
             toStringImpl(node->right) + ")";
    case ExprType::Mul:
      return "(" + toStringImpl(node->left) + " * " +
             toStringImpl(node->right) + ")";
    case ExprType::Div:
      return "(" + toStringImpl(node->left) + " / " +
             toStringImpl(node->right) + ")";
    case ExprType::Pow:
      return "(" + toStringImpl(node->left) + "^" + toStringImpl(node->right) +
             ")";
    case ExprType::Sin:
      return "sin(" + toStringImpl(node->left) + ")";
    case ExprType::Cos:
      return "cos(" + toStringImpl(node->left) + ")";
    case ExprType::Ln:
      return "ln(" + toStringImpl(node->left) + ")";
    case ExprType::Exp:
      return "exp(" + toStringImpl(node->left) + ")";
    }

    return "";
  }

  static std::shared_ptr<ExprNode<T>>
  substituteImpl(const std::shared_ptr<ExprNode<T>> &node,
                 const std::string &varName, const T &val) {
    if (!node)
      return nullptr;

    if (node->type == ExprType::Variable && node->varName == varName) {

      return std::make_shared<ExprNode<T>>(ExprType::Constant, val);
    }

    auto newNode = std::make_shared<ExprNode<T>>(*node);
    newNode->left = substituteImpl(node->left, varName, val);
    newNode->right = substituteImpl(node->right, varName, val);
    return newNode;
  }

  static T evaluateImpl(const std::shared_ptr<ExprNode<T>> &node,
                        const std::map<std::string, T> &varValues) {
    if (!node) {
      throw std::runtime_error("Cannot evaluate an empty node");
    }

    switch (node->type) {
    case ExprType::Constant:
      return node->value;

    case ExprType::Variable: {
      auto it = varValues.find(node->varName);
      if (it == varValues.end()) {
        throw std::runtime_error("Missing value for variable: " +
                                 node->varName);
      }
      return it->second;
    }

    case ExprType::Add:
      return evaluateImpl(node->left, varValues) +
             evaluateImpl(node->right, varValues);

    case ExprType::Sub:
      return evaluateImpl(node->left, varValues) -
             evaluateImpl(node->right, varValues);

    case ExprType::Mul:
      return evaluateImpl(node->left, varValues) *
             evaluateImpl(node->right, varValues);

    case ExprType::Div: {
      T denominator = evaluateImpl(node->right, varValues);
      if (std::fabs(denominator) < 1e-15) {
        throw std::runtime_error("Division by zero");
      }
      T numerator = evaluateImpl(node->left, varValues);
      return numerator / denominator;
    }

    case ExprType::Pow: {
      T base = evaluateImpl(node->left, varValues);
      T exponent = evaluateImpl(node->right, varValues);

      return std::pow(base, exponent);
    }

    case ExprType::Sin:
      return std::sin(evaluateImpl(node->left, varValues));

    case ExprType::Cos:
      return std::cos(evaluateImpl(node->left, varValues));

    case ExprType::Ln: {
      T arg = evaluateImpl(node->left, varValues);
      if (arg <= (T)0) {
        throw std::runtime_error("ln domain error: argument <= 0");
      }
      return std::log(arg);
    }

    case ExprType::Exp:
      return std::exp(evaluateImpl(node->left, varValues));
    }
    throw std::runtime_error("Unknown expression type in evaluateImpl()");
  }

  static std::shared_ptr<ExprNode<T>>
  differentiateImpl(const std::shared_ptr<ExprNode<T>> &node,
                    const std::string &varName) {
    if (!node)
      return nullptr;

    switch (node->type) {
    case ExprType::Constant:

      return std::make_shared<ExprNode<T>>(ExprType::Constant, (T)0);

    case ExprType::Variable:

      if (node->varName == varName) {
        return std::make_shared<ExprNode<T>>(ExprType::Constant, (T)1);
      } else {
        return std::make_shared<ExprNode<T>>(ExprType::Constant, (T)0);
      }

    case ExprType::Add: {

      auto leftDiff = differentiateImpl(node->left, varName);
      auto rightDiff = differentiateImpl(node->right, varName);
      return std::make_shared<ExprNode<T>>(ExprType::Add, leftDiff, rightDiff);
    }
    case ExprType::Sub: {

      auto leftDiff = differentiateImpl(node->left, varName);
      auto rightDiff = differentiateImpl(node->right, varName);
      return std::make_shared<ExprNode<T>>(ExprType::Sub, leftDiff, rightDiff);
    }
    case ExprType::Mul: {

      auto leftDiff = differentiateImpl(node->left, varName);
      auto rightDiff = differentiateImpl(node->right, varName);

      auto part1 =
          std::make_shared<ExprNode<T>>(ExprType::Mul, leftDiff, node->right);
      auto part2 =
          std::make_shared<ExprNode<T>>(ExprType::Mul, node->left, rightDiff);
      return std::make_shared<ExprNode<T>>(ExprType::Add, part1, part2);
    }
    case ExprType::Div: {

      auto leftDiff = differentiateImpl(node->left, varName);
      auto rightDiff = differentiateImpl(node->right, varName);

      auto numeratorPart1 =
          std::make_shared<ExprNode<T>>(ExprType::Mul, leftDiff, node->right);
      auto numeratorPart2 =
          std::make_shared<ExprNode<T>>(ExprType::Mul, node->left, rightDiff);
      auto numerator = std::make_shared<ExprNode<T>>(
          ExprType::Sub, numeratorPart1, numeratorPart2);

      auto two = std::make_shared<ExprNode<T>>(ExprType::Constant, (T)2);
      auto denom =
          std::make_shared<ExprNode<T>>(ExprType::Pow, node->right, two);

      return std::make_shared<ExprNode<T>>(ExprType::Div, numerator, denom);
    }
    case ExprType::Pow: {

      if (node->right && node->right->type == ExprType::Constant) {

        T c = node->right->value;

        auto cNode = std::make_shared<ExprNode<T>>(ExprType::Constant, c);

        auto cMinusOneNode =
            std::make_shared<ExprNode<T>>(ExprType::Constant, c - (T)1);

        auto newPow = std::make_shared<ExprNode<T>>(ExprType::Pow, node->left,
                                                    cMinusOneNode);

        auto front =
            std::make_shared<ExprNode<T>>(ExprType::Mul, cNode, newPow);

        auto baseDiff = differentiateImpl(node->left, varName);
        return std::make_shared<ExprNode<T>>(ExprType::Mul, front, baseDiff);
      } else {

        auto uDiff = differentiateImpl(node->left, varName);
        auto vDiff = differentiateImpl(node->right, varName);

        auto uPowv = std::make_shared<ExprNode<T>>(ExprType::Pow, node->left,
                                                   node->right);

        auto lnU =
            std::make_shared<ExprNode<T>>(ExprType::Ln, node->left, nullptr);
        auto partA = std::make_shared<ExprNode<T>>(ExprType::Mul, vDiff, lnU);

        auto partB_top =
            std::make_shared<ExprNode<T>>(ExprType::Mul, node->right, uDiff);
        auto partB =
            std::make_shared<ExprNode<T>>(ExprType::Div, partB_top, node->left);

        auto inside =
            std::make_shared<ExprNode<T>>(ExprType::Add, partA, partB);
        return std::make_shared<ExprNode<T>>(ExprType::Mul, uPowv, inside);
      }
    }
    case ExprType::Sin: {

      auto uDiff = differentiateImpl(node->left, varName);
      auto cosU =
          std::make_shared<ExprNode<T>>(ExprType::Cos, node->left, nullptr);
      return std::make_shared<ExprNode<T>>(ExprType::Mul, cosU, uDiff);
    }
    case ExprType::Cos: {

      auto uDiff = differentiateImpl(node->left, varName);
      auto sinU =
          std::make_shared<ExprNode<T>>(ExprType::Sin, node->left, nullptr);
      auto negOne = std::make_shared<ExprNode<T>>(ExprType::Constant, (T)-1);
      auto minusSinU =
          std::make_shared<ExprNode<T>>(ExprType::Mul, negOne, sinU);
      return std::make_shared<ExprNode<T>>(ExprType::Mul, minusSinU, uDiff);
    }
    case ExprType::Ln: {

      auto uDiff = differentiateImpl(node->left, varName);
      return std::make_shared<ExprNode<T>>(ExprType::Div, uDiff, node->left);
    }
    case ExprType::Exp: {

      auto uDiff = differentiateImpl(node->left, varName);
      auto expU =
          std::make_shared<ExprNode<T>>(ExprType::Exp, node->left, nullptr);
      return std::make_shared<ExprNode<T>>(ExprType::Mul, expU, uDiff);
    }
    }

    return nullptr;
  }
};

#endif