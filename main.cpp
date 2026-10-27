#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iomanip>
#include "fields.h"

enum class OpType { Read, Write, String };

struct Op {
    OpType type = OpType::Read;
    int field = 0;
    int value = 0;
};

struct OpList {
    std::unique_ptr<Op[]> data;
    int size = 0;
};

OpList readOps(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);

    int count = 0;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) ++count;
    }

    in.clear();
    in.seekg(0);

    OpList list;
    list.data = std::make_unique<Op[]>(count);
    list.size = count;

    std::string cmd;
    for (int i = 0; i < count; ++i) {
        if (!(in >> cmd)) throw std::runtime_error("unexpected end of " + path);

        Op& op = list.data[i];
        if (cmd == "read") {
            op.type = OpType::Read;
            in >> op.field;
        } else if (cmd == "write") {
            op.type = OpType::Write;
            in >> op.field >> op.value;
        } else if (cmd == "string") {
            op.type = OpType::String;
        } else {
            throw std::runtime_error("unknown command: " + cmd);
        }

        if (op.field < 0 || op.field >= Fields::m) throw std::runtime_error("bad field index in " + path);
    }
    return list;
}

void worker(Fields& f, const Op* ops, int count, long long& out) {
    long long sink = 0;
    for (int i = 0; i < count; i++) {
        const Op& op = ops[i];
        switch (op.type) {
            case OpType::Read: sink += f.get(op.field); break;
            case OpType::Write: f.set(op.field, op.value); break;
            case OpType::String: sink += std::string(f).size(); break;
        }
    }
    out = sink;
}

double runThreads(const OpList& ops, int k) {
    Fields f;
    auto threads = std::make_unique<std::thread[]>(k);
    auto results = std::make_unique<long long[]>(k);
    int part = ops.size / k;

    auto start = std::chrono::steady_clock::now();
    for (int t = 0; t < k; ++t) {
        int count = (t == k - 1) ? ops.size - t * part : part;
        threads[t] = std::thread(worker, std::ref(f), ops.data.get() + t * part, count, std::ref(results[t]));
    }
    for (int t = 0; t < k; ++t) {
        threads[t].join();
    }
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::milli>(end - start).count();
}

const int RUNS = 10;
double averageTime(const OpList& ops, int k) {
    double total = 0;
    for (int r = 0; r < RUNS; ++r) {
        total += runThreads(ops, k);
    }
    return total / RUNS;
}

void processFile(const std::string& name, const std::string& path) {
    OpList ops = readOps(path);
    std::cout << std::left << std::setw(14) << name << std::right;
    for (int k = 1; k <= 3; ++k) {
        std::cout << std::setw(12) << averageTime(ops, k);
    }
    std::cout << '\n';
}

int main() {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Average time, ms (" << RUNS << " runs each)\n";
    std::cout << std::left << std::setw(14) << "file" << std::right << std::setw(12) << "1 thread" 
    << std::setw(12) << "2 threads" << std::setw(12) << "3 threads" << '\n';

    try {
        processFile("a) variant", "data/a_variant.txt");
        processFile("b) equal", "data/b_equal.txt");
        processFile("c) mismatch", "data/c_mismatch.txt");
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << '\n';
        return 0;
    }
}