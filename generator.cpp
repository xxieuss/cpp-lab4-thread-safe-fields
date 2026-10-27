#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

void generate(const std::string& path, std::initializer_list<double> weights, int n) {
    std::mt19937 gen(std::random_device{}());
    std::discrete_distribution<int> dist(weights);
    std::ofstream out(path);

    for (int i = 0; i < n; ++i) {
        switch (dist(gen)) {
            case 0: out << "read 0\n"; break;
            case 1: out << "write 0 1\n"; break;
            case 2: out << "read 1\n"; break;
            case 3: out << "write 1 1\n"; break;
            case 4: out << "string\n"; break;
        }
    }
    std::cout << "generated " << path << '\n';
}

int main() {
    const int n = 3'000'000;
    std::filesystem::create_directories("data");

    generate("data/a_variant.txt",  {20, 5, 20, 5, 50}, n);
    generate("data/b_equal.txt",    {20, 20, 20, 20, 20}, n);
    generate("data/c_mismatch.txt", {5, 40, 5, 40, 10}, n);
}