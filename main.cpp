#include <iostream>
#include <string>
#include "fields.h"

int main() {
    Fields f;
    f.set(0, 5);
    f.set(1, 7);
    std::cout << f.get(0) << " " << f.get(1) << "\n";
    std::cout << std::string(f) << "\n";
}