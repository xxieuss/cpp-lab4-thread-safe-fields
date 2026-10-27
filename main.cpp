#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <stdexcept>
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

void processFile(const std::string& path) {
    OpList ops = readOps(path);
    std::cout << path << ": " << ops.size << " ops\n";
}

int main() {
    try {
        processFile("data/a_variant.txt");
        processFile("data/b_equal.txt");
        processFile("data/c_mismatch.txt");
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << '\n';
        return 0;
    }
}