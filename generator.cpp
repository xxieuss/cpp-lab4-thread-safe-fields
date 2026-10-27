#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

void generate(const std::string& path, int r0, int w0, int r1, int w1, int n) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 99);
    std::ofstream out(path);

    for (int i = 0; i < n; ++i) {
        int x = dist(gen);
        if (x < r0) out << "read 0\n";
        else if (x < r0 + w0) out << "write 0 1\n";
        else if (x < r0 + w0 + r1) out << "read 1\n";
        else if (x < r0 + w0 + r1 + w1) out << "write 1 1\n";
        else out << "string\n";
    }
    std::cout << "generated " << path << '\n';
}

int main() {
    const int n = 3'000'000;
    std::filesystem::create_directories("data");

    generate("data/a_variant.txt", 20, 5, 20, 5, n);
    generate("data/b_equal.txt", 20, 20, 20, 20, n);
    generate("data/c_mismatch.txt", 5, 40, 5, 40, n);
}