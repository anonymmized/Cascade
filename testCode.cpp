#include <iostream>
#include <string>

int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

int division(int a, int b) {
    return a / b;
}

int subtraction(int a, int b) {
    return a - b;
}

// MAIN
void printAll(int a, int b) {
    std::cout << "Add: " << add(a, b) << '\n';
    std::cout << "Substraction: " << subtraction(a, b) << '\n';
    std::cout << "Division: " << division(a, b) << '\n';
    std::cout << "Multiply: " << multiply(a, b) << '\n';
    std::cout << "The program ended\n";
}
