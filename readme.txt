This project is  C++ library for symbolic differentiation, which is for:
Arithmetic operations on expressions (+, -, *, /, ^)
Mathematical functions (sin, cos, ln, exp)
Expression evaluation at given variable values
Expression differentiation with respect to a variable
Parsing expressions from strings
Command-line execution for evaluation and differentiation
Automated testing to ensure correctness

For differentiator:

./differentiator --eval "x * y" x=10 y=12

For symbolic derivatives:

./differentiator --diff "x * sin(x)" --by x

Testing the Library:

make clean
make
make run_tests



for manual testingS:

./differentiator --eval "-5"
./differentiator --eval "-x" x=3
./differentiator --eval "-(x + y)" x=2 y=3
./differentiator --diff "-x" --by x
./differentiator --diff "-sin(x)" --by x

./differentiator --diff "x^2" --by x
./differentiator --diff "x^3 + 2*x" --by x
./differentiator --diff "sin(x) + cos(x)" --by x
./differentiator --diff "ln(x)" --by x

./differentiator --eval "2 * x + 3 * y" x=2 y=4
./differentiator --eval "sin(pi/2)"
./differentiator --eval "exp(ln(5))"
./differentiator --eval "x^2 / (x + 1)" x=2

./differentiator --eval "1/0"
./differentiator --eval "ln(-5)"
./differentiator --eval "x + y" x=2
./differentiator --eval "x + * y"
