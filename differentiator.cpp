#include "differentiator.hpp"
#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef TESTING
auto main(int argc, char *argv[]) -> int {
  std::cout << "Running differentiator normally..." << '\n';

  std::vector<std::string> args;
  args.reserve(static_cast<std::size_t>(argc - 1)); 
  for (int i = 1; i < argc; i++) {
    args.emplace_back(argv[i]);
  }

  bool doEval = false;
  bool doDiff = false;
  std::string expressionStr;
  std::string diffVar;

  for (size_t i = 0; i < args.size(); i++) {
    if (args[i] == "--eval") {
      doEval = true;
      if (i + 1 < args.size()) {
        expressionStr = args[i + 1];
      } else {
        std::cerr << "Error: --eval requires an expression.\n";
        return 1;
      }
    } else if (args[i] == "--diff") {
      doDiff = true;
      if (i + 1 < args.size()) {
        expressionStr = args[i + 1];
      } else {
        std::cerr << "Error: --diff requires an expression.\n";
        return 1;
      }
    } else if (args[i] == "--by") {
      if (i + 1 < args.size()) {
        diffVar = args[i + 1];
      } else {
        std::cerr << "Error: --by requires a variable.\n";
        return 1;
      }
    }
  }

  if ((doEval ^ doDiff) == 0) {
    std::cerr << "Usage:\n"
              << "  differentiator --eval \"expr\" x=val y=val ...\n"
              << "  differentiator --diff \"expr\" --by var\n";
    return 1;
  }

  try {

    using ExprD = Expression<double>;

    if (doEval) {

      ExprD const expr = ExprD::parse(expressionStr);

      std::map<std::string, double> varMap;
      for (const auto &arg : args) {
        auto pos = arg.find('=');
        if (pos != std::string::npos) {
          std::string const varName = arg.substr(0, pos);
          std::string const valStr = arg.substr(pos + 1);
          double const val = std::stod(valStr);
          varMap[varName] = val;
        }
      }

      double const result = expr.evaluate(varMap);
      std::cout << result << "\n";

    } else if (doDiff) {

      ExprD const expr = ExprD::parse(expressionStr);

      if (diffVar.empty()) {
        std::cerr << "Error: must specify --by var\n";
        return 1;
      }

      ExprD const derivative = expr.differentiate(diffVar);
      std::cout << derivative.toString() << "\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
#endif