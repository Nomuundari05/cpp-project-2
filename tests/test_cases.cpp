#include <iostream>
#include <map>
#include <cmath>
#include <string>
#include <complex>

#include <stdexcept>
#include "../differentiator.hpp"
#define TESTING
#include "../differentiator.cpp"

static int g_totalTests = 0;
static int g_failedTests = 0;

void checkTest(bool condition, const std::string& testName) {
    g_totalTests++;
    if (condition) {
        std::cout << testName << ": OK\n";
    } else {
        g_failedTests++;
        std::cout << testName << ": FAIL\n";
    }
}

void testConstructors() {
    {

        Expression<double> e1(5.8);
        checkTest(std::fabs(e1.evaluate() - 5.8) < 1e-9, "Constructor from constant double");
    }
    {

        Expression<double> e2("x");
        std::map<std::string, double> vals = {{"x", 2.5}};
        checkTest(std::fabs(e2.evaluate(vals) - 2.5) < 1e-9, "Constructor from variable");
    }
    {

        Expression<double> original(3.14);
        Expression<double> copyConstructed(original);
        checkTest(std::fabs(copyConstructed.evaluate() - 3.14) < 1e-9, "Copy constructor");
    }
    {

        Expression<double> temp(2.71);
        Expression<double> moved(std::move(temp));
        checkTest(std::fabs(moved.evaluate() - 2.71) < 1e-9, "Move constructor");
    }
}

void testArithmetic() {
    using E = Expression<double>;

    E a(5.0), b(10.0);
    E sum = a + b;
    checkTest(std::fabs(sum.evaluate() - 15.0) < 1e-9, "Addition test");

    E c(3.0);
    E sub = b - c;
    checkTest(std::fabs(sub.evaluate() - 7.0) < 1e-9, "Subtraction test");

    E d(6.0), e(4.0);
    E mul = d * e;
    checkTest(std::fabs(mul.evaluate() - 24.0) < 1e-9, "Multiplication test");

    E f(8.0), g(2.0);
    E div = f / g;
    checkTest(std::fabs(div.evaluate() - 4.0) < 1e-9, "Division test");

    E h(2.0), i(3.0);
    E pw = h ^ i;
    checkTest(std::fabs(pw.evaluate() - 8.0) < 1e-9, "Power test");
}

void testFunctions() {
    using E = Expression<double>;

    E pi_over_2(M_PI / 2.0);
    E s = sin(pi_over_2);
    checkTest(std::fabs(s.evaluate() - 1.0) < 1e-9, "sin test");

    E pi(M_PI);
    E c = cos(pi);
    checkTest(std::fabs(c.evaluate() + 1.0) < 1e-9, "cos test");

    E eVal(M_E);
    E l = ln(eVal);
    checkTest(std::fabs(l.evaluate() - 1.0) < 1e-9, "ln test");

    E one(1.0);
    E ex = exp(one);
    checkTest(std::fabs(ex.evaluate() - M_E) < 1e-9, "exp test");
}

void testSubstitutionAndEvaluation() {
    using E = Expression<double>;

    E x("x");
    E y("y");
    E expr = x + y;

    E subExpr = expr.substitute("x", 2.0);
    std::map<std::string, double> vals = {{"y", 3.0}};
    double result = subExpr.evaluate(vals);
    checkTest(std::fabs(result - 5.0) < 1e-9, "Substitution test (x=2) => x+y, y=3 => 5");
}

void testToString() {
    using E = Expression<double>;

    E a(5.0), x("x"), b(3.0);
    E expr = (a + x) * b;

    std::string str = expr.toString();
    bool ok = (str.find("5") != std::string::npos) &&
              (str.find("x") != std::string::npos) &&
              (str.find("+") != std::string::npos) &&
              (str.find("*") != std::string::npos);
    checkTest(ok, "toString test (check presence of tokens)");
}

void testParsing() {
    using E = Expression<double>;

    E parsed = E::parse("2*x");
    std::map<std::string, double> vals = {{"x", 10.0}};
    double result = parsed.evaluate(vals);
    checkTest(std::fabs(result - 20.0) < 1e-9, "parse('2*x') evaluate with x=10 => 20");

    E parsed2 = E::parse("sin(x)+3");
    std::map<std::string, double> vals2 = {{"x", M_PI/2}};
    double result2 = parsed2.evaluate(vals2);

    std::cout << "DEBUG: Parsed expression for 'sin(x) + 3': " << parsed2.toString() << "\n";
    std::cout << "DEBUG: Evaluated result for x=pi/2: " << result2 << "\n";

    checkTest(std::fabs(result2 - 4.0) < 1e-9, "parse('sin(x)+3') evaluate => 4");
}

void testDifferentiation() {
    using E = Expression<double>;

    E x("x");
    E expr = x * x;
    E dx = expr.differentiate("x");
    std::string dStr = dx.toString();
    bool ok = (dStr.find("x") != std::string::npos &&
               (dStr.find("+") != std::string::npos || dStr.find(" + ") != std::string::npos));
    checkTest(ok, "Differentiate x*x wrt x => derivative should contain 'x + x'");

    E expr2 = x * sin(x);
    E dx2 = expr2.differentiate("x");
    std::string dStr2 = dx2.toString();

    bool ok2 = (dStr2.find("x") != std::string::npos &&
                dStr2.find("cos") != std::string::npos &&
                dStr2.find("sin") != std::string::npos &&
                dStr2.find("+") != std::string::npos);
    checkTest(ok2, "Differentiate x*sin(x) wrt x => x*cos(x) + sin(x)");
}

int runAllTests() {

    g_totalTests = 0;
    g_failedTests = 0;

    std::cout << "==== Running Tests ====\n";

    testConstructors();
    testArithmetic();
    testFunctions();
    testSubstitutionAndEvaluation();
    testToString();
    testParsing();
    testDifferentiation();

    std::cout << "==== Tests Complete ====\n";
    std::cout << "Total tests: " << g_totalTests << "\n";
    std::cout << "Failed tests: " << g_failedTests << "\n";

    return (g_failedTests > 0) ? 1 : 0;
}