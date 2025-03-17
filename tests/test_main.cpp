#include <iostream>


int runAllTests();

int main() {

    int result = runAllTests();
    if (result == 0) {
        std::cout << "All tests passed.\n";
    } else {
        std::cout << "Some tests failed.\n";
    }
    return result;
}
