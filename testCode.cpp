

#include <iostream>
#include <string>

// MAIN
void printAll(int a, int b) {
    std::cout << "Add: " << add(a, b) << '\n';
    std::cout << "Substraction: " << subtraction(a, b) << '\n';
    std::cout << "Division: " << division(a, b) << '\n';
    std::cout << "Multiply: " << multiply(a, b) << '\n';
    std::cout << "The program ended\n";
}

int add(int a, int b) {
    int sum = doSum(a, b);
    return sum;
}

int doSum(int a, int b) {
    return a + b;
}

int subtraction(int a, int b) {
    return a - b;
}

int division(int a, int b) {
    return a / b;
}

int multiply(int a, int b) {
    return a * b;
}



