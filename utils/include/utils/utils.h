#pragma once
#include <string>
#include <cmath>
#include <vector>
#include <map>

namespace utils {
    std::string formatNumber(long double number);
    void stringReplace(std::string &source, const std::string &original, const std::string &replacement);
    std::string quotedString(const std::string &source, const std::string &quote);
    bool isInteger(long double number, long double tolerance = 1e-9);

    template<typename K, typename V>
    std::vector<K> mapKeys(const std::map<K, V>& map) {
        std::vector<K> keys;
        for (const auto& pair : map) {
            const auto key = std::get<0>(pair);
            keys.push_back(key);
        }
        return keys;
    }

    template<typename K, typename V>
    std::vector<V> mapValues(const std::map<K, V>& map) {
        std::vector<V> values;
        for (const auto& pair : map) {
            const auto value = std::get<1>(pair);
            values.push_back(value);
        }
        return values;
    }

    template<typename T>
    bool compareVectors(const std::vector<T> first, const std::vector<T> second) {
        if (first.size() != second.size()) return false;
        for (size_t i = 0; i < first.size(); i++) {
            if (first[i] != second[i]) return false;
        }
        return true;
    }
}