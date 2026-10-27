#pragma once
#include <mutex>
#include <shared_mutex>
#include <string>

class Fields {
public:
    static constexpr int m = 2;

    Fields() = default;
    Fields(const Fields&) = delete;
    Fields& operator=(const Fields&) = delete;
    ~Fields() = default;

    int get(int i) const {
        std::shared_lock lock(fields[i].mtx);
        return fields[i].value;
    }

    void set(int i, int value) {
        std::lock_guard lock(fields[i].mtx);
        fields[i].value = value;
    }

    operator std::string() const {
        int v0, v1;
        {
            std::shared_lock lock0(fields[0].mtx);
            std::shared_lock lock1(fields[1].mtx);
            v0 = fields[0].value;
            v1 = fields[1].value;
        }
        return std::to_string(v0) + " " + std::to_string(v1);
    }

private:
    struct Field {
        int value = 0;
        mutable std::shared_mutex mtx;
    };
    Field fields[m];
};